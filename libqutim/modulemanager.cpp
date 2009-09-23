#include "modulemanager.h"
#include "plugin.h"
#include "deprecatedplugin_p.h"
#include "cryptoservice.h"
#include "configbase_p.h"
#include "protocol.h"
#include <QPluginLoader>
#include <QSettings>
#include <QDir>
#include <QApplication>
#include <QSet>
#include <QPointer>
#include <QMetaMethod>
#include <QDebug>
#include <QVarLengthArray>

namespace qutim_sdk_0_3
{
	const char *qutimVersion()
	{
		return QUTIM_VERSION_STR;
	}

	const char *metaInfo(const QMetaObject *meta, const char *name)
	{
		int num = meta ? meta->classInfoCount() : 0;
		while(num --> 0)
		{
			QMetaClassInfo info = meta->classInfo(num);
			if(!qstrcmp(info.name(), name))
				return info.value();
		}
		return 0;
	}

	class ModuleManagerPrivate
	{
	public:
		inline ModuleManagerPrivate() : is_inited(false), protocols_hash(new QHash<QString, QPointer<Protocol> >()) {}
		inline ~ModuleManagerPrivate() { delete protocols_hash; }
		QList<QPointer<Plugin> > plugins;
		bool is_inited;
		union {
			QHash<QString, QPointer<Protocol> > *protocols_hash;
			QHash<QString, Protocol *> *protocols;
		};
	};

	bool isCoreInited()
	{
		return ModuleManager::self && ModuleManager::self->p && ModuleManager::self->p->is_inited;
	}

	GeneratorList moduleGenerators(const QMetaObject *module)
	{
		GeneratorList list;
		if(isCoreInited())
		{
			QMultiMap<Plugin *, ExtensionInfo> exts = ModuleManager::self->getExtensions(module);
			QMultiMap<Plugin *, ExtensionInfo>::const_iterator it = exts.constBegin();
			for(; it != exts.constEnd(); it++)
				list << it.value().generator();
		}
		return list;
	}

	ProtocolMap allProtocols()
	{
		ProtocolMap map;
		if(isCoreInited())
			map = *ModuleManager::self->p->protocols;
		return map;
	}

	QString statusToString(Status status, bool translate)
	{
		const char *str;
		switch(status)
		{
		default:
			if(status >= Connecting)
				return translate ? QCoreApplication::translate("Status", "Connecting") : QLatin1String("Connecting");
		case Online:
			return translate ? QCoreApplication::translate("Status", "Online") : QLatin1String("Online");
		case Offline:
			return translate ? QCoreApplication::translate("Status", "Offline") : QLatin1String("Offline");
		case Away:
			return translate ? QCoreApplication::translate("Status", "Away") : QLatin1String("Away");
		case DND:
			return translate ? QCoreApplication::translate("Status", "Don't disturb") : QLatin1String("DND");
		case NA:
			return translate ? QCoreApplication::translate("Status", "Not avaiable") : QLatin1String("NA");
		case Occupied:
			return translate ? QCoreApplication::translate("Status", "Occupied") : QLatin1String("Occupied");
		case FreeChat:
			return translate ? QCoreApplication::translate("Status", "Free for chat") : QLatin1String("FreeChat");
		case Evil:
			return translate ? QCoreApplication::translate("Status", "Evil") : QLatin1String("Evil");
		case Depression:
			return translate ? QCoreApplication::translate("Status", "Depression") : QLatin1String("Depression");
		case Invisible:
			return translate ? QCoreApplication::translate("Status", "Invisible") : QLatin1String("Invisible");
		case AtHome:
			return translate ? QCoreApplication::translate("Status", "At home") : QLatin1String("AtHome");
		case AtWork:
			return translate ? QCoreApplication::translate("Status", "At work") : QLatin1String("AtWork");
		case OnThePhone:
			return translate ? QCoreApplication::translate("Status", "On the phone") : QLatin1String("OnThePhone");
		case OutToLunch:
			return translate ? QCoreApplication::translate("Status", "Out to lunch") : QLatin1String("OutToLunch");
		}
	}

	ModuleManager *ModuleManager::self = 0;

	ModuleManager::ModuleManager(QObject *parent) : QObject(parent)
	{
		qDebug() << QIcon::themeSearchPaths();
		Q_ASSERT_X(!self, "ModuleManager", "Only one instance of ModuleManager can be created");
		p = new ModuleManagerPrivate;
		self = this;
		qApp->setApplicationName("qutIM");
		qApp->setApplicationVersion(qutimVersion());
		qApp->setOrganizationDomain("qutim.org");
//		qApp->setOrganizationName("qutIM");
	}

	ModuleManager::~ModuleManager()
	{
		ModuleManager::self = 0;
	}

	void ModuleManager::loadPlugins(const QStringList &additional_paths)
	{
		QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim", "qutimsettings");

		QStringList paths = additional_paths;
		QDir root_dir = QApplication::applicationDirPath();
		// 1. Windows, ./plugins
		QString plugin_path = root_dir.canonicalPath();
		plugin_path += QDir::separator();
		plugin_path += "plugins";
		paths << plugin_path;
		root_dir.cdUp();
		// 2. Linux, /usr/lib/qutim
		// May be it should be changed to /usr/lib/qutim/plugins ?..
		plugin_path = root_dir.canonicalPath();
		plugin_path += QDir::separator();
		plugin_path += "lib";
		plugin_path += QDir::separator();
		plugin_path += "qutim";
		paths << plugin_path;
		plugin_path += QDir::separator();
		plugin_path += "plugins";
		paths << plugin_path;
		// 3. MacOS X, ../PlugIns
		plugin_path = root_dir.canonicalPath();
		plugin_path += QDir::separator();
		plugin_path += "PlugIns";
		paths << plugin_path;
		// 4. Safe way, ~/.config/qutim/plugins
		plugin_path = QFileInfo(settings.fileName()).canonicalPath();
		plugin_path += QDir::separator();
		plugin_path += "plugins";
		paths << plugin_path;
		// 5. From config
		QStringList config_paths = settings.value("General/libpaths", QStringList()).toStringList();
		paths << config_paths;
		paths.removeDuplicates();
		QSet<QString> plugin_paths_list;
		foreach(const QString &path, paths)
		{
			QDir plugins_dir = path;
			QFileInfoList files = plugins_dir.entryInfoList(QDir::AllEntries);
			for(int i = 0; i < files.count(); ++i)
			{
				QString filename = files[i].canonicalFilePath();
				if(plugin_paths_list.contains(filename) || !QLibrary::isLibrary(filename) || !files[i].isFile())
					continue;
				plugin_paths_list << filename;
				QPluginLoader *loader = new QPluginLoader(filename);
				loader->setLoadHints(QLibrary::ExportExternalSymbolsHint);
				QObject *object = loader->instance();
				Plugin *plugin = qobject_cast<Plugin *>(object);
//				if(!plugin)
//					plugin = createDeprecatedPlugin(object);
				if(plugin)
				{
					qDebug("%s", qPrintable(filename));
					plugin->init();
					p->plugins.append(plugin);
				}
				else
				{
					delete object;
					loader->unload();
				}
			}
		}
	}

	QMultiMap<Plugin *, ExtensionInfo> ModuleManager::getExtensions(const QMetaObject *service_meta) const
	{
		QMultiMap<Plugin *, ExtensionInfo> result;
		if(!service_meta)
			return result;
		for(int i = -1; i < p->plugins.size(); i++)
		{
			Plugin *plugin;
			QList<ExtensionInfo> extensions;
			if(i < 0)
			{
				plugin = 0;
				extensions = self->coreExtensions();
			}
			else
			{
				plugin = p->plugins.at(i);
				qDebug() << plugin;
				if(!plugin)
					continue;
				qDebug() << plugin->metaObject()->className();
				extensions = plugin->avaiableExtensions();
			}
			for(int j = 0; j < extensions.size(); j++)
			{
				qDebug() << extensions.at(j).generator()->metaObject()->className() << extensions.at(j).generator()->extends(service_meta);
				if(extensions.at(j).generator()->extends(service_meta))
					result.insert(plugin, extensions.at(j));
			}
		}
		return result;
	}

	QObject *ModuleManager::initExtension(const QMetaObject *service_meta)
	{
		QMultiMap<Plugin *, ExtensionInfo> exts = getExtensions(service_meta);
		QMultiMap<Plugin *, ExtensionInfo>::const_iterator it = exts.begin();
		for(; it != exts.end(); it++)
		{
			QObject *obj = it.value().generator()->generate();
			qDebug("Found %s for %s", it.value().generator()->metaObject()->className(), service_meta->className());
			return obj;
		}
		qWarning("%s extension isn't found", service_meta->className());
		return 0;
	}

	void ModuleManager::initExtensions()
	{
		CryptoService::self = initExtension<CryptoService>();
		{
			QMultiMap<Plugin *, ExtensionInfo> exts = getExtensions<ConfigBackend>();
			QMultiMap<Plugin *, ExtensionInfo>::const_iterator it = exts.begin();
			for(; it != exts.end(); it++)
			{
				const QMetaObject *meta = it.value().generator()->metaObject();
				QByteArray name = metaInfo(meta, "Extension");
				if(name.isEmpty())
				{
					qWarning("%s has no 'Extension' class info", meta->className());
					continue;
				}
				ConfigPrivate::config_backends << qMakePair(name, it.value().generator()->generate<ConfigBackend>());
			}
		}
		{
			QMultiMap<Plugin *, ExtensionInfo> exts = getExtensions<Protocol>();
			QMultiMap<Plugin *, ExtensionInfo>::const_iterator it = exts.begin();
			for(; it != exts.end(); it++)
			{
				const QMetaObject *meta = it.value().generator()->metaObject();
				QByteArray name = metaInfo(meta, "Protocol");
				if(name.isEmpty())
				{
					qWarning("%s has no 'Protocol' class info", meta->className());
					continue;
				}
				Protocol *protocol = it.value().generator()->generate<Protocol>();
				p->protocols_hash->insert(protocol->id(), protocol);
			}
		}
		p->is_inited = true;
		foreach(Protocol *proto, allProtocols())
			proto->loadAccounts();
	}
}

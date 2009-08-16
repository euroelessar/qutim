#include "modulemanager.h"
#include "plugin.h"
#include "deprecatedplugin_p.h"
#include "cryptoservice.h"
#include "configbase_p.h"
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

	class ModuleManagerPrivate
	{
	public:
		QList<QPointer<Plugin> > plugins;
	};

	ModuleManager *ModuleManager::self = 0;

	ModuleManager::ModuleManager(QObject *parent) : QObject(parent)
	{
		Q_ASSERT_X(!self, "ModuleManager", "Only one instance of ModuleManager can be created");
		p = new ModuleManagerPrivate;
		self = this;
		qApp->setApplicationName("qutIM");
		qApp->setApplicationVersion(qutimVersion());
		qApp->setOrganizationDomain("qutim.org");
		qApp->setOrganizationName("qutIM");
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
				qDebug("%s", qPrintable(filename));
				QPluginLoader *loader = new QPluginLoader(filename);
				QObject *object = loader->instance();
				Plugin *plugin = qobject_cast<Plugin *>(object);
				if(!plugin)
					plugin = createDeprecatedPlugin(object);
				if(plugin)
					p->plugins.append(plugin);
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
				if(!plugin)
					continue;
				extensions = plugin->avaiableExtensions();
			}
			for(int j = 0; j < extensions.size(); j++)
			{
				const QMetaObject *meta = extensions.at(j).meta();
				while(meta && (meta = meta->superClass()) != service_meta);
				if(meta == service_meta)
					result.insert(plugin, extensions.at(j));
			}
		}
		return result;
	}

	inline QObject *create_instance_helper(const QMetaObject *meta)
	{
#if QT_VERSION < QT_VERSION_CHECK(4, 5, 3)
		QByteArray constructorName = meta->className();
		{
			int idx = constructorName.lastIndexOf(':');
			if (idx != -1)
				constructorName.remove(0, idx+1); // remove qualified part
		}
		constructorName.append("()\0", 3);
		int idx = meta->indexOfConstructor(constructorName.constData());
		if(idx < 0)
			qWarning("%s doesn't have invokable constructor", meta->className());
		QVariant ret(QMetaType::QObjectStar, (void*)0);
		void *param[] = {ret.data(), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

		if (meta->static_metacall(QMetaObject::CreateInstance, idx, param) >= 0)
			return 0;
		return *reinterpret_cast<QObject**>(param[0]);
#else
		return meta->newInstance();
#endif
	}

	template<typename T>
	inline T *create_instance(const QMetaObject *meta)
	{
		if(T *result = qobject_cast<T *>(create_instance_helper(meta)))
		{
			qDebug("Found %s for %s", meta->className(), T::staticMetaObject.className());
			return result;
		}
		qWarning("%s doesn't have invokable constructor", meta->className());
		return 0;
	}

	QObject *ModuleManager::initExtension(const QMetaObject *service_meta)
	{
		QMultiMap<Plugin *, ExtensionInfo> exts = getExtensions(service_meta);
		QMultiMap<Plugin *, ExtensionInfo>::const_iterator it = exts.begin();
		for(; it != exts.end(); it++)
		{
			const QMetaObject *meta = it.value().meta();
			if(QObject *obj = create_instance_helper(meta))
			{
				qDebug("Found %s for %s", meta->className(), service_meta->className());
				return obj;
			}
			else
			{
				qWarning("%s doesn't have invokable constructor", meta->className());
				continue;
			}
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
				const QMetaObject *meta = it.value().meta();
				QByteArray name;
				for(int i = 0; i < meta->classInfoCount(); i++)
				{
					QMetaClassInfo info = meta->classInfo(i);
					if(!qstrcmp(info.name(), "Extension"))
					{
						name = info.value();
						break;
					}
				}
				if(name.isEmpty())
				{
					qWarning("%s has no 'Extension' class info", meta->className());
					continue;
				}
				if(ConfigBackend *object = create_instance<ConfigBackend>(it.value().meta()))
					ConfigPrivate::config_backends << qMakePair(name, object);
			}
		}
	}
}

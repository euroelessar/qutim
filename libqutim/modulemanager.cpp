#include "modulemanager.h"
#include "plugin.h"
#include "deprecatedplugin_p.h"
#include <QPluginLoader>
#include <QSettings>
#include <QDir>
#include <QApplication>
#include <QSet>
#include <QPointer>

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
		for(int i = 0; i < p->plugins.size(); i++)
		{
			Plugin *plugin = p->plugins.at(i);
			if(!plugin)
				continue;
			QList<ExtensionInfo> extensions = plugin->avaiableExtensions();
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
}

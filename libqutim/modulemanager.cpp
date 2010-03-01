/****************************************************************************
 *  modulemanager.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "modulemanager.h"
#include "plugin_p.h"
#include "deprecatedplugin_p.h"
#include "cryptoservice.h"
#include "configbase_p.h"
#include "protocol.h"
#include "contactlist.h"
#include "notificationslayer.h"
#include <QPluginLoader>
#include <QSettings>
#include <QDir>
#include <QApplication>
#include <QSet>
#include <QPointer>
#include <QMetaMethod>
#include <QDebug>
#include <QVarLengthArray>
#include <QLibrary>

// Is there any other way to init CryptoService from ModuleManager?
#define INSIDE_MODULE_MANAGER
#include "cryptoservice.cpp"

namespace qutim_sdk_0_3
{
	const char *qutimVersionStr()
	{
		return QUTIM_VERSION_STR;
	}

	quint32 qutimVersion()
	{
		return QUTIM_VERSION;
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

	enum ModuleFlag
	{
	};
	Q_DECLARE_FLAGS(ModuleFlags, ModuleFlag)

	/**
	 * ModuleManagerPrivate class used to hide inner structure of ModuleManager to provide binary compatibility between different versions.
	 */
	class ModuleManagerPrivate
	{
	public:
		inline ModuleManagerPrivate() : is_inited(false), protocols_hash(new QHash<QString, QPointer<Protocol> >()) {}
		inline ~ModuleManagerPrivate() { delete protocols_hash; }
		QList<QPointer<Plugin> > plugins;
		bool is_inited;
		union { // This union is intended to be used as reinterpret_cast =)
			QHash<QString, QPointer<Protocol> > *protocols_hash;
			QHash<QString, Protocol *> *protocols;
		};
		QHash<QString, QHash<QString, ModuleFlags> > choosed_modules;
		QSet<QByteArray> interface_modules;
		QSet<const QMetaObject *> meta_modules;
	};

	/**
	 * Function to detect if ModuleManager and it's inner data had been initialized.
	 */
	bool isCoreInited()
	{
		return ModuleManager::self && ModuleManager::self->p && ModuleManager::self->p->is_inited;
	}

	/**
	 * Returns list of ObjectGenerator's for extensions that match QMetaObject criterion
	 */
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

	/**
	 * Returns list of ObjectGenerator's for extensions that match char* interfaceID
	 */
	GeneratorList moduleGenerators(const char *iid)
	{
		GeneratorList list;
		if(isCoreInited())
		{
			QMultiMap<Plugin *, ExtensionInfo> exts = ModuleManager::self->getExtensions(iid);
			QMultiMap<Plugin *, ExtensionInfo>::const_iterator it = exts.constBegin();
			for(; it != exts.constEnd(); it++)
				list << it.value().generator();
		}
		return list;
	}

	/**
	 * Returns Map list of protocols
	 */
	ProtocolMap allProtocols()
	{
		ProtocolMap map;
		if(isCoreInited())
			map = *ModuleManager::self->p->protocols;
		return map;
	}

	/**
	 * The only instance of ModuleManager (singleton)
	 */
	ModuleManager *ModuleManager::self = 0;

	/**
	 * This is ModuleManager constructor.
	 */
	ModuleManager::ModuleManager(QObject *parent) : QObject(parent)
	{
		qDebug() << QIcon::themeSearchPaths();
		Q_ASSERT_X(!self, "ModuleManager", "Only one instance of ModuleManager can be created");
		p = new ModuleManagerPrivate;
		self = this;
		qApp->setApplicationName("qutIM");
		qApp->setApplicationVersion(qutimVersionStr());
		qApp->setOrganizationDomain("qutim.org");
//		qApp->setOrganizationName("qutIM");
	}

	/**
	 * This is ModuleManager destructor.
	 */
	ModuleManager::~ModuleManager()
	{
		ModuleManager::self = 0;
	}

	/**
	 * This function used to search and load plugins.
	 */
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
			for (int i = 0; i < files.count(); ++i) {
				QString filename = files[i].canonicalFilePath();
				if(plugin_paths_list.contains(filename) || !QLibrary::isLibrary(filename) || !files[i].isFile())
					continue;
				plugin_paths_list << filename;
				// Just don't load old plugins
				{
#ifdef Q_CC_BOR
					typedef const char * __stdcall (*QutimPluginVerificationFunction)();
#else
					typedef const char * (*QutimPluginVerificationFunction)();
#endif
					QutimPluginVerificationFunction verificationFunction = NULL;
					QScopedPointer<QLibrary> lib(new QLibrary(filename));
					if (lib->load()) {
						verificationFunction = (QutimPluginVerificationFunction)lib->resolve("qutim_plugin_query_verification_data");
						lib->unload();
						if (!verificationFunction) {
//							qDebug("'%s' has no valid verification data", qPrintable(filename));
							continue;
						}
					} else {
						qDebug("%s", qPrintable(lib->errorString()));
						continue;
					}
				}
				QPluginLoader *loader = new QPluginLoader(filename);
				if (filename.contains("quetzal"))
					loader->setLoadHints(QLibrary::ExportExternalSymbolsHint);
				QObject *object = loader->instance();
				if (Plugin *plugin = qobject_cast<Plugin *>(object)) {
					plugin->info().data()->fileName = filename;
					plugin->init();
					if (plugin->p->validate()) {
						plugin->p->is_inited = true;
						p->plugins.append(plugin);
					} else {
						delete object;
					}
				} else {
					if (object)
						delete object;
					else
						qWarning("%s", qPrintable(loader->errorString()));
					loader->unload();
				}
			}
		}
	}

	/**
	 * Selects all available extensions by QMetaObject criterion
	 */
	QMultiMap<Plugin *, ExtensionInfo> ModuleManager::getExtensions(const QMetaObject *service_meta) const
	{
		p->meta_modules.insert(service_meta);
		QMultiMap<Plugin *, ExtensionInfo> result;
		if (!service_meta)
			return result;
		for (int i = -1; i < p->plugins.size(); i++) {
			Plugin *plugin;
			ExtensionInfoList extensions;
			if (i < 0) {
				plugin = 0;
				extensions = self->coreExtensions();
			} else {
				plugin = p->plugins.at(i);
				if(!plugin)
					continue;
				extensions = plugin->avaiableExtensions();
			}
			for (int j = 0; j < extensions.size(); j++)
			{
				if(extensions.at(j).generator()->extends(service_meta))
					result.insert(plugin, extensions.at(j));
			}
		}
		return result;
	}

	/**
	 * Selects all available extensions by char* interface id criterion
	 */
	QMultiMap<Plugin *, ExtensionInfo> ModuleManager::getExtensions(const char *interface_id) const
	{
		QMultiMap<Plugin *, ExtensionInfo> result;
		if(!interface_id)
			return result;
		for(int i = -1; i < p->plugins.size(); i++)
		{
			Plugin *plugin;
			ExtensionInfoList extensions;
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
				if(extensions.at(j).generator()->extends(interface_id))
					result.insert(plugin, extensions.at(j));
			}
		}
		return result;
	}

	/**
	 * Initializes specific extension. To select extension type QMetaObject used.
	 */
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

	/**
	 * Don't know
	 */
	void ModuleManager::initExtensions()
	{
		qutim_sdk_0_3::self = initExtension<CryptoService>();
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
				else
				{
					qDebug("Found '%s' for '%s'", meta->className(), name.constData());
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
				else
					qDebug("Found protocol '%s'", name.constData());
				Protocol *protocol = it.value().generator()->generate<Protocol>();
				p->protocols_hash->insert(protocol->id(), protocol);
			}
		}
		p->is_inited = true;
		{
			QMultiMap<Plugin *, ExtensionInfo> exts = getExtensions(qobject_interface_iid<StartupModule *>());
			QMultiMap<Plugin *, ExtensionInfo>::const_iterator it = exts.begin();
			for(; it != exts.end(); it++)
			{
				qDebug("Startup: %s", it.value().generator()->metaObject()->className());
				it.value().generator()->generate<StartupModule>();
			}
		}
		foreach(Plugin *plugin, p->plugins) {
			if (plugin)
				plugin->load();
		}

		foreach(Protocol *proto, allProtocols())
			proto->loadAccounts();
		Q_UNUSED(ContactList::instance());
		Notifications::sendNotification(Notifications::Startup, 0);
	}
}

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include <qglobal.h>
#include "modulemanager_p.h"
#include "plugin_p.h"
#include "cryptoservice.h"
#include "config.h"
#include "notification.h"
#include "systeminfo.h"
#include "metacontactmanager.h"
#include "metaobjectbuilder.h"
#include "protocol.h"
#include "servicemanager.h"
#include "startupmodule.h"
#include "icon.h"
#include "varianthook_p.h"
#include "debug.h"
#include "servicemanager_p.h"
#include "libqutim_version.h"
#include "sound_p.h"
#include <QPluginLoader>
#include <QSettings>
#include <QDir>
#include <QApplication>
#include <QMetaMethod>
#include <QDebug>
#include <QVarLengthArray>
#include <QLibrary>
#include <QDesktopServices>
#include <QTime>
#include <QQueue>
#include "objectgenerator.h"

// Is there any other way to init CryptoService from ModuleManager?
#define INSIDE_MODULE_MANAGER
#include "cryptoservice.cpp"

#define QUTIM_TEST_PERFOMANCE 1

//Let's show message box with error
#if	defined(Q_OS_SYMBIAN)
# undef QUTIM_TEST_PERFOMANCE
# include <QMessageBox>
# include <QLibraryInfo>
#endif

#define NO_COMMANDS 1

namespace qutim_sdk_0_3
{
LIBQUTIM_EXPORT QList<ConfigBackend*> &get_config_backends();

// Static Fields
static ModuleManager *managerSelf = NULL;
static ModuleManagerPrivate *d = NULL;

ExtensionInfoList extensionList()
{
    return (managerSelf && d && d->is_inited) ? d->extensions : ExtensionInfoList();
}

QList<QWeakPointer<Plugin> > pluginsList()
{
    return (managerSelf && d && d->is_inited) ? d->plugins : QList<QWeakPointer<Plugin> >();
}

bool isCoreInited()
{
    return managerSelf && d && d->is_inited;
}

GeneratorList moduleGenerators(const QMetaObject *module, const char *iid)
{
	Q_ASSERT((module == 0) ^ (iid == 0));
	GeneratorList list;
    if (!managerSelf || !d)
		return list;
	if (module && !iid)
		iid = module->className();
	const QByteArray id = QByteArray::fromRawData(iid, qstrlen(iid));
    ExtensionNode *node = d->nodes.value(id);
	if (!node)
		return list;
	// BFS
	QSet<ExtensionInfo::Data*> used;
	QQueue<ExtensionNode*> queue;
	queue.enqueue(node);
	while (!queue.isEmpty()) {
		node = queue.dequeue();
		for (int i = 0; i < node->infos.size(); ++i) {
			if (!used.contains(node->infos.at(i).data())) {
				used.insert(node->infos.at(i).data());
				list << node->infos.at(i).generator();
			}
		}
		for (int i = 0; i < node->children.size(); ++i)
				queue.enqueue(node->children.at(i));
	}
	return list;
}

ProtocolHash allProtocols()
{
    ProtocolHash map;
	if(ObjectGenerator::isInited())
        map = d->protocols;
	return map;
}

static ExtensionNode *ensureNode(const QMetaObject *meta)
{
	QByteArray id = QByteArray::fromRawData(meta->className(), qstrlen(meta->className()));
    ExtensionNodeHash::Iterator it = d->nodes.find(id);
    if (it != d->nodes.end())
		return it.value();
	ExtensionNode *parent = meta->superClass() ? ensureNode(meta->superClass()) : 0;
	id.detach();
    ExtensionNode *node = d->nodes.insert(id, new ExtensionNode).value();
	if (parent)
		parent->children << node;
	return node;
}

static ExtensionNode *ensureNode(const QByteArray &id)
{
    ExtensionNodeHash::Iterator it = d->nodes.find(id);
    if (it != d->nodes.end())
		return it.value();
    return d->nodes.insert(id, new ExtensionNode).value();
}

static void addExtension(const ExtensionInfo &info)
{
	const QMetaObject *meta = info.generator()->metaObject();
	debug() << meta->className();
	Q_ASSERT(meta);
	ensureNode(meta)->infos << info;
	const QList<QByteArray> interfaces = info.generator()->interfaces();
	for (int i = 0; i < interfaces.size(); ++i)
		ensureNode(interfaces.at(i))->infos << info;
}

#ifndef NO_COMMANDS
QString formatVersion(quint32 version)
{
	QString str;
	str += QString::number((version & 0xff000000) >> 24);
	str += QLatin1Char('.');
	str += QString::number((version & 0x00ff0000) >> 16);
	str += QLatin1Char('.');
	str += QString::number((version & 0x0000ff00) >> 8);
	str += QLatin1Char('.');
	str += QString::number( version & 0x000000ff);
	return str;
}

static void printVersion()
{
	QString version;
	QTextStream str(&version);
	str << "\n  " << qApp->applicationName() << ' ' << versionString()
		<< " based on Qt " << qVersion() << "\n\n";
	foreach (Plugin *plugin, p->plugins) {
		PluginInfo info = plugin->info();
		str << "  " << info.name() << ' ' << formatVersion(info.version())
			<< ' ' << info.description() <<  '\n';
	}

	str << "\n  " << "GPL v2 or any later" << '\n';
	debug() << version;
}

static void printHelp(const QList<CommandArgumentsHandler*> &handlers)
{
}
#endif // NO_COMMANDS

static bool checkQtPluginData(const char *data, QString *error)
{
	Q_UNUSED(data);
	Q_UNUSED(error);
	return true;
}

static bool checkQutIMPluginData(const char *data, QString *error)
{
	Q_UNUSED(data);
	Q_UNUSED(error);
	return true;
}

/**
  * This is ModuleManager constructor.
  */
ModuleManager::ModuleManager(QObject *parent) : QObject(parent)
{
	debug() << QIcon::themeSearchPaths();
	Q_ASSERT_X(!managerSelf, "ModuleManager", "Only one instance of ModuleManager can be created");
	VariantHook::init();
	qRegisterMetaTypeStreamOperators<Status>();
    d = new ModuleManagerPrivate;
	managerSelf = this;
	qApp->setApplicationName("qutIM");
	qApp->setApplicationVersion(versionString());
	qApp->setOrganizationDomain("qutim.org");
	//		qApp->setOrganizationName("qutIM");
	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(onQuit()));
}

/**
  * This is ModuleManager destructor.
  */
ModuleManager::~ModuleManager()
{
	managerSelf = 0;
}

/**
  * This function used to search and load plugins.
  */
void ModuleManager::loadPlugins(const QStringList &additional_paths)
{
	// Static plugins
	foreach (QObject *object, QPluginLoader::staticInstances()) {
		if (Plugin *plugin = qobject_cast<Plugin *>(object)) {
			plugin->init();
            if (plugin->p->validate()) {
                plugin->p->info.data()->inited = 1;
                d->plugins.append(plugin);
                d->extensions << plugin->avaiableExtensions();
			}
		}
	}

	QStringList paths;
    d->extensions << coreExtensions();

#if	defined(Q_OS_SYMBIAN)
	// simple S60 plugins loader
	QDir pluginsDir(QLibraryInfo::location(QLibraryInfo::PluginsPath));
	paths << pluginsDir.filePath("qutim");

//	foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
//		// Create plugin loader
//		QPluginLoader* loader = new QPluginLoader(pluginsDir.absoluteFilePath(fileName));
//		// Load plugin
//		if (!loader->load()) {
//			QMessageBox msg;
//			msg.setText(tr("Could not load plugin: \n %1").arg(fileName));
//			msg.exec();
//			delete loader;
//			continue;
//		}
//		// init plugin
//		QObject *object = loader->instance();
#else // defined(Q_OS_SYMBIAN)
	paths = additional_paths;
	QDir root_dir = QApplication::applicationDirPath();
//	1. Windows, ./plugins
	QString plugin_path = root_dir.canonicalPath();
	plugin_path += QDir::separator();
	plugin_path += "plugins";
	paths << plugin_path;
	root_dir.cdUp();

//	2. Linux, /usr/lib/qutim/plugins
	plugin_path = root_dir.canonicalPath();
	plugin_path += QDir::separator();
	plugin_path += "lib";
	plugin_path += QDir::separator();
	plugin_path += "qutim";
	plugin_path += QDir::separator();
	plugin_path += "plugins";
	paths << plugin_path;

//	2.5 Some Linux system, 64 bit, /usr/lib64/qutim/plugins
	plugin_path = root_dir.canonicalPath();
	plugin_path += QDir::separator();
	plugin_path += "lib64";
	plugin_path += QDir::separator();
	plugin_path += "qutim";
	plugin_path += QDir::separator();
	plugin_path += "plugins";
	paths << plugin_path;

//	3. MacOS X, ../PlugIns
#ifdef Q_OS_MAC
	plugin_path = root_dir.canonicalPath();
	plugin_path += QDir::separator();
	plugin_path += "PlugIns";
	paths << plugin_path;
#endif // Q_OS_MAC
	
// 4. Safe way, ~/.local/share/qutim/plugins
//		plugin_path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
//		plugin_path += QDir::separator();
//		plugin_path += "plugins";
//		paths << plugin_path;
//		// 6. From config
//		QStringList config_paths = settings.value("General/libpaths", QStringList()).toStringList();
//		paths << config_paths;
	
#endif // defined(Q_OS_SYMBIAN)

	paths.removeDuplicates();
	QSet<QString> pluginPathsList;

	foreach (const QString &path, paths) {
		QDir plugins_dir = path;
		QFileInfoList files = plugins_dir.entryInfoList(QDir::AllEntries);
		QFileInfoList nextTry;
		forever {
			for (int i = 0; i < files.count(); ++i) {
				QString filename = files[i].canonicalFilePath();
				if(pluginPathsList.contains(filename) || !QLibrary::isLibrary(filename) || !files[i].isFile())
					continue;
				pluginPathsList << filename;
#ifndef Q_OS_SYMBIAN
				// Just don't load old plugins
				typedef const char * Q_STANDARD_CALL (*QutimPluginVerificationFunction)();
				typedef const char * Q_STANDARD_CALL (*QtPluginVerificationFunction)();
				QutimPluginVerificationFunction verificationFunction = NULL;
				QtPluginVerificationFunction qtVerificationFunction = NULL;
#ifdef QUTIM_TEST_PERFOMANCE
				QTime timer;
				int libLoadTime, verifyTime, instanceTime, initTime;
#endif // QUTIM_TEST_PERFOMANCE
				{
#ifdef QUTIM_TEST_PERFOMANCE
					timer.start();
#endif // QUTIM_TEST_PERFOMANCE
					QScopedPointer<QLibrary> lib(new QLibrary(filename));
					if (lib->load()) {
#ifdef QUTIM_TEST_PERFOMANCE
						libLoadTime = timer.elapsed();
						timer.restart();
#endif // QUTIM_TEST_PERFOMANCE
						verificationFunction = reinterpret_cast<QutimPluginVerificationFunction>(
						            lib->resolve("qutim_plugin_query_verification_data"));
						qtVerificationFunction = reinterpret_cast<QtPluginVerificationFunction>(
						            lib->resolve("qt_plugin_query_verification_data"));
#ifdef QUTIM_TEST_PERFOMANCE
						verifyTime = timer.elapsed();
						timer.restart();
#endif // QUTIM_TEST_PERFOMANCE
						if (!verificationFunction || !qtVerificationFunction) {
							lib->unload();
							debug() << filename << " has no valid verification data";
							continue;
						}
						QString error;
						if (!checkQtPluginData(qtVerificationFunction(), &error)
						        || !checkQutIMPluginData(verificationFunction(), &error)) {
							debug() << "Error while loading plugin " << filename << ": " << error;
						}
					} else {
						debug() << lib->errorString();
						nextTry << files[i];
						pluginPathsList.remove(filename);
						continue;
					}
				}
#endif // Q_OS_SYMBIAN
				QPluginLoader *loader = new QPluginLoader(filename);
				QObject *object = loader->instance();
#ifdef QUTIM_TEST_PERFOMANCE
				instanceTime = timer.elapsed(); timer.restart();
#endif // QUTIM_TEST_PERFOMANCE
				
				if (Plugin *plugin = qobject_cast<Plugin *>(object)) {
					plugin->init();
#ifdef QUTIM_TEST_PERFOMANCE
					initTime = timer.elapsed();
					debug() << files[i].fileName() << ":\nload:" << libLoadTime << "ms, verify:" << verifyTime
							<< "ms, instance:" << instanceTime << "ms, init:" << initTime << "ms";
#endif // QUTIM_TEST_PERFOMANCE
                    if (plugin->p->validate()) {
                        plugin->p->info.data()->inited = 1;
                        d->plugins.append(plugin);
                        d->extensions << plugin->avaiableExtensions();
						foreach(ExtensionInfo info, plugin->avaiableExtensions())
                            d->extsPlugins.insert(info.name(), plugin);
					} else {
						delete object;
					}
				} else {
					if (object)
						delete object;
					else {
						warning() << loader->errorString();
#ifdef Q_OS_SYMBIAN
						QMessageBox msg;
						msg.setText(tr("Could not init plugin: \n %1").arg(loader->errorString()));
						msg.exec();
#endif
					}
					loader->unload();
				}
			}
			if (!nextTry.isEmpty() && nextTry.size() != files.size()) {
				qSwap(nextTry, files);
				nextTry.clear();
			} else {
				break;
			}
		}
	}

#ifndef NO_COMMANDS	
	QStringList args = qApp->arguments();
	if (args.size() > 1) {
		QList<CommandArgumentsHandler*> handlers;
		foreach (Plugin *plugin, p->plugins) {
			CommandArgumentsHandler *handler = qobject_cast<CommandArgumentsHandler*>(plugin);
			if (handler)
				handlers << handler;
		}

		QStringList::iterator it = args.begin();
		QStringList::iterator itend = args.end();
		// Skip program name
		it++;
		for (; it != itend; it++) {
			if (*it == QLatin1String("-v") || *it == QLatin1String("--version")) {
				printVersion();
				qApp->quit();
				return;
			}
			if (*it == QLatin1String("-h") || *it == QLatin1String("--help")) {
				printHelp(handlers);
				qApp->quit();
				return;
			}
		}
	}
#endif // NO_COMMANDS

    foreach (const ExtensionInfo &info, d->extensions) {
		addExtension(info);
        d->extensionsHash.insert(info.generator()->metaObject()->className(), info);
	}
}

ExtensionInfoList ModuleManager::extensions(const char *iid) const
{
	ExtensionInfoList list;
	const QByteArray id = QByteArray::fromRawData(iid, qstrlen(iid));
    ExtensionNode *node = d->nodes.value(id);
	if (!node)
		return list;
	// BFS
	QSet<ExtensionInfo::Data*> used;
	QQueue<ExtensionNode*> queue;
	queue.enqueue(node);
	while (!queue.isEmpty()) {
		node = queue.dequeue();
		for (int i = 0; i < node->infos.size(); ++i) {
			if (!used.contains(node->infos.at(i).data())) {
				used.insert(node->infos.at(i).data());
				list << node->infos.at(i);
			}
		}
		for (int i = 0; i < node->children.size(); ++i)
			queue.enqueue(node->children.at(i));
	}
	return list;
}

ExtensionInfoList ModuleManager::extensions(const QMetaObject *meta) const
{
	return extensions(meta->className());
}

/**
  * Initializes specific extension. To select extension type QMetaObject used.
  */
QObject *ModuleManager::initExtension(const QMetaObject *meta)
{
	const ExtensionInfoList exts = extensions(meta);
	for (int i = 0; i < exts.size(); ++i) {
		const ObjectGenerator *generator = exts.at(i).generator();
		QObject *obj = generator->generate();
		debug() << QString("Found %1 for %2").arg(generator->metaObject()->className(), meta->className());
		return obj;
	}
	warning() << meta->className() << " extension isn't found";
	return 0;
}

/**
  * Don't know
  */
void ModuleManager::initExtensions()
{
	Q_UNUSED(Sound::instance());
	// TODO: remove old API and this hack
	QList<ConfigBackend*> &configBackends = get_config_backends();
	if (configBackends.isEmpty()) {
		const ExtensionInfoList exts = extensions(ConfigBackend::staticMetaObject.className());
		for (int i = 0; i < exts.size(); ++i) {
			const ExtensionInfo &info = exts.at(i);
			const QMetaObject *meta = info.generator()->metaObject();
			QByteArray name = MetaObjectBuilder::info(meta, "Extension");
			if (name.isEmpty()) {
				warning() << meta->className() << " has no 'Extension' class info";
				continue;
			}
			debug() << "Found " << meta->className() << " for " << name.constData();
			configBackends << info.generator()->generate<ConfigBackend>();
		}
	}
	QSet<PluginInfo::Data*> disabledPlugins;
	Config pluginsConfig;
	pluginsConfig.beginGroup("plugins/list");
	{
        foreach (QWeakPointer<Plugin> plugin, d->plugins) {
            if (!pluginsConfig.value(plugin.data()->metaObject()->className(), true))
                disabledPlugins << plugin.data()->info().data();
		}
        for (int i = 0; i < d->extensions.size(); i++) {
            PluginInfo::Data *data = d->extensions.at(i).data()->plugin.data();
			if (disabledPlugins.contains(data)) {
                d->extensions.removeAt(i);
				i--;
				break;
			}
		}
	}

	QSet<QByteArray> usedExtensions;
	{
        const QHash<QByteArray, ExtensionInfo> &extsHash = d->extensionsHash;
		ConfigGroup group = Config().group("protocols");
		QVariantMap selected = group.value("list", QVariantMap());
		bool changed = false;
		QVariantMap::const_iterator it = selected.constBegin();
		for (; it != selected.constEnd(); it++) {
			const ExtensionInfo info = extsHash.value(it.value().toString().toLatin1());
			//				Plugin *plugin = p->extsPlugins.value(info.name());
			//				if(plugin)
			//					if (!pluginsConfig.value(plugin->metaObject()->className(), true))
			//						continue;

			if (info.generator() && info.generator()->extends<Protocol>()) {
				const QMetaObject *meta = info.generator()->metaObject();
				QByteArray name = MetaObjectBuilder::info(meta, "Protocol");
				if (name.isEmpty() || name != it.key())
					continue;
				debug() << name << meta->className();
				Protocol *protocol = info.generator()->generate<Protocol>();
                d->protocols.insert(protocol->id(), protocol);
				usedExtensions << meta->className();

                connect(protocol, SIGNAL(destroyed(QObject*)), this, SLOT(_q_protocolDestroyed(QObject*)));
			}
		}
		const ExtensionInfoList exts = extensions(Protocol::staticMetaObject.className()); //p->extensions;
		ExtensionInfoList::const_iterator it2 = exts.constBegin();
		for(; it2 != exts.end(); it2++) {
			//				Plugin *plugin = p->extsPlugins.value(it2->name());
			//				if (plugin)
			//					if (!pluginsConfig.value<bool>(plugin->metaObject()->className(), true))
			//						continue;

			const ObjectGenerator *gen = it2->generator();
			const QMetaObject *meta = gen->metaObject();
			QString name = QLatin1String(MetaObjectBuilder::info(meta, "Protocol"));
            if (name.isEmpty() || d->protocols.contains(name))
				continue;
			Protocol *protocol = gen->generate<Protocol>();
            d->protocols.insert(protocol->id(), protocol);
			usedExtensions << meta->className();
			selected.insert(protocol->id(), QString::fromLatin1(meta->className()));
			changed = true;

            connect(protocol, SIGNAL(destroyed(QObject*)), this, SLOT(_q_protocolDestroyed(QObject*)));
		}
		if (changed) {
			group.setValue("list", selected);
			group.sync();
		}
	}
    d->is_inited = true;
    for (int i = 0; i < d->extensions.size(); i++) {
        const QMetaObject *meta = d->extensions.at(i).generator()->metaObject();
		for (int j = 0; j < meta->classInfoCount(); j++) {
			QMetaClassInfo info = meta->classInfo(j);
			if (!qstrcmp(info.name(), "DependsOn") && !usedExtensions.contains(info.value())) {
                d->extensions.removeAt(i);
				i--;
				break;
			}
		}
	}
	ServiceManagerPrivate::get(ServiceManager::instance())->init();
#ifndef Q_OS_MAC
	qApp->setWindowIcon(Icon("qutim"));
#endif

	{
		const ExtensionInfoList exts = extensions(qobject_interface_iid<StartupModule *>());
		for (int i = 0; i < exts.size(); ++i) {
			//				Plugin *plugin = it.key();
			//				if (!pluginsConfig.value(plugin->metaObject()->className(), true))
			//					continue;
#ifdef QUTIM_TEST_PERFOMANCE
			QTime timer;
			timer.start();
#endif
			exts.at(i).generator()->generate<StartupModule>();
#ifdef QUTIM_TEST_PERFOMANCE
			debug() << "Startup:" << exts.at(i).generator()->metaObject()->className() << "," << timer.elapsed() << "ms";
#endif
		}
	}

	foreach(Protocol *proto, Protocol::all()) {
#ifdef QUTIM_TEST_PERFOMANCE
		QTime timer;
		timer.start();
#endif
		proto->loadAccounts();
#ifdef QUTIM_TEST_PERFOMANCE
		debug() << proto->id() << ", load:" << timer.elapsed() << "ms";
#endif
	}

	if (MetaContactManager *manager = MetaContactManager::instance())
		manager->loadContacts();

    for (int i = 0; i < d->plugins.size(); i++) {
        Plugin *plugin = d->plugins.at(i).data();
		//			if (plugin && pluginsConfig.value(plugin->metaObject()->className(), true)) {
		if (plugin && !disabledPlugins.contains(plugin->info().data())) {
			if (plugin->info().capabilities() & Plugin::Loadable) {
#ifdef QUTIM_TEST_PERFOMANCE
				QTime timer;
				timer.start();
#endif
				if (plugin->load()) {
					plugin->info().data()->loaded = 1;
				} else {
#ifdef QUTIM_TEST_PERFOMANCE
					debug() << plugin->metaObject()->className() <<  ", load:" << timer.elapsed() << "ms";
#endif
					continue;
				}
#ifdef QUTIM_TEST_PERFOMANCE
				debug() << plugin->metaObject()->className() << ", load:" << timer.elapsed() << "ms";
#endif
				if (PluginFactory *factory = qobject_cast<PluginFactory*>(plugin)) {
					QList<Plugin*> plugins = factory->loadPlugins();
					for (int j = 0; j < plugins.size(); j++) {
						Plugin *subPlugin = plugins.at(j);
						if (!pluginsConfig.value(subPlugin->metaObject()->className(), true))
							disabledPlugins << subPlugin->info().data();
						subPlugin->init();
                        d->plugins << subPlugin;
					}
				}
			}
		}
		debug() << i << d->plugins.size() << d->plugins.at(i).data()->metaObject()->className();
	}
	Event("startup").send();
}

void ModuleManager::onQuit()
{
	Event("aboutToQuit").send();
    foreach(QWeakPointer<Plugin> plugin, d->plugins) {
        if (!plugin.isNull() && plugin.data()->info().data()->loaded) {
            plugin.data()->unload();
		}
        delete plugin.data();
	}
	ServiceManagerPrivate::get(ServiceManager::instance())->deinit();

    foreach (QString key, d->protocols.keys())
        d->protocols.take(key)->deleteLater();
}

void ModuleManager::_q_protocolDestroyed(QObject *obj)
{
    QString key = d->protocols.key(static_cast<Protocol*>(obj));
    d->protocols.remove(key);
}

void ModuleManager::virtual_hook(int id, void *data)
{
	Q_UNUSED(id);
	Q_UNUSED(data);
}
}

#include "modulemanager.moc"

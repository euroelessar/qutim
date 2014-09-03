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
#include "profile.h"
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
#include <QUrl>
#include <qendian.h>
#include "objectgenerator.h"

// Is there any other way to init CryptoService from ModuleManager?
#define INSIDE_MODULE_MANAGER
#include "cryptoservice.cpp"

#define QUTIM_TEST_PERFOMANCE 1

#include <QCommandLineParser>

//Let's show message box with error
#if	defined(Q_OS_SYMBIAN)
# undef QUTIM_TEST_PERFOMANCE
# include <QMessageBox>
# include <QLibraryInfo>
#endif

//#define NO_COMMANDS 1

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

QList<QPointer<Plugin> > pluginsList()
{
    return (managerSelf && d && d->is_inited) ? d->plugins : QList<QPointer<Plugin> >();
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
	qDebug() << meta->className();
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
	QTextStream str(stdout);
	str << qApp->applicationName() << " version " << versionString() << endl
		<< "Using Qt version " << qVersion() << endl
		<< "GPL v3 or any later" << endl;
//	foreach (Plugin *plugin, p->plugins) {
//		PluginInfo info = plugin->info();
//		str << "  " << info.name() << ' ' << formatVersion(info.version())
//			<< ' ' << info.description() <<  '\n';
//	}
}
#endif // NO_COMMANDS

struct InfoToken
{
	const char *key;
	uint keyLength;
	const char *value;
	uint valueLength;
};

bool scanNextInfoToken(InfoToken &token, const char * &data)
{
	while (isspace(*data))
		++data;
	if (!*data)
		return false;
	token.key = token.value = 0;
	token.keyLength = token.valueLength = 0;
	token.key = data;
	while (*data && *data != '=')
		++data;
	token.keyLength = (data - token.key);
	if (!*data)
		return false;
	++data;
	token.value = data;
	while (*data && *data != '\n')
		++data;
	token.valueLength = (data - token.value);
	return true;
}

static bool checkQutIMPluginData(const char *data, quint64 *debugId, QString *error)
{
	InfoToken token;
	bool isValidPattern = false;
	bool isValidQutimVersion = false;
	while (scanNextInfoToken(token, data)) {
		if (!qstrncmp("pattern", token.key, token.keyLength)) {
			isValidPattern = !qstrncmp("QUTIM_PLUGIN_VERIFICATION_DATA", token.value, token.valueLength);
			if (!isValidPattern)
				break;
		} else if (!qstrncmp("debugid", token.key, token.keyLength)) {
			if (token.valueLength != 16) {
				*error = QLatin1String("Invalid plugin identification number");
				return false;
			}
			QByteArray data = QByteArray::fromRawData(token.value, token.valueLength);
			QByteArray number = QByteArray::fromHex(data);
			if (number.size() != 8) {
				*error = QLatin1String("Invalid plugin identification number");
				return false;
			}
			*debugId = qFromBigEndian<quint64>(reinterpret_cast<const uchar *>(number.constData()));
		} else if (!qstrncmp("libqutim", token.key, token.keyLength)) {
			isValidQutimVersion = token.valueLength == qstrlen(versionString())
								  && !qstrncmp(versionString(), token.value, token.valueLength);
		}
	}
	if (!isValidPattern)
		*error = QLatin1String("There is no valid qutIM's plugin verification data");
	else if (!isValidQutimVersion)
		*error = QLatin1String("Plugin is built with incompatible libqutim's version");
	return isValidPattern && isValidQutimVersion;
}

void ModuleManagerPrivate::initLocalPeer(const QString &message, bool *shouldExit)
{
	*shouldExit = false;
	d->localPeer.reset(new QtLocalPeer(managerSelf));
	if (d->localPeer->isClient()) {
		if (d->localPeer->sendMessage(message, 1000)) {
			*shouldExit = true;
			return;
		}
		// Assume we became a server
		Q_ASSERT(!d->localPeer->isClient());
	}
	QObject::connect(d->localPeer.data(), SIGNAL(messageReceived(QString)),
					 managerSelf, SLOT(_q_messageReceived(QString)));
}

void ModuleManager::_q_messageReceived(const QString &message)
{
	if (message.startsWith(QLatin1String("arguments: "))) {
		QByteArray data = QByteArray::fromBase64(message.section(QLatin1Char(' '), 1).toLatin1());
		QDataStream s(&data, QIODevice::ReadOnly);
		QStringList args;
		s >> args;
		qDebug() << "Received message with:" << args;
		ServicePointer<QObject> contactList("ContactList");
		if (contactList)
			QMetaObject::invokeMethod(contactList.data(), "show", Qt::QueuedConnection);
	} else if (message.startsWith(QLatin1String("url: "))) {
		QUrl url(message.section(QLatin1Char(' '), 1));
		QDesktopServices::openUrl(url);
	}
}

/**
  * This is ModuleManager constructor.
  */
ModuleManager::ModuleManager(QObject *parent) : QObject(parent)
{
	qDebug() << QIcon::themeSearchPaths();
	Q_ASSERT_X(!managerSelf, "ModuleManager", "Only one instance of ModuleManager can be created");
	VariantHook::init();
	qRegisterMetaType<QObject*>();
	qRegisterMetaType<QAction*>("QAction*");
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
	delete d;
}

/**
  * This function used to search and load plugins.
  */
void ModuleManager::loadPlugins(const QStringList &additional_paths)
{
#ifndef NO_COMMANDS
	const QStringList args = qApp->arguments();

	QCommandLineParser parser;
	QCommandLineOption help(QStringList() << "h" << "?" << "help", "This help message");
	parser.addOption(help);

	QCommandLineOption version(QStringList() << "v" << "version", "Show version information");
	parser.addOption(version);

	//parser.addPositionalArgument("url", "Url to open (Like xmpp:rabbit@40k.org?message)");

	//parser.add("single-instance", "Run single instance", QxtCommandOptions::NoValue, 0x15);
	//parser.add("new-instance", "Run new instance", QxtCommandOptions::NoValue, 0x15);
	QCommandLineOption singleInst("single-instance", "Run single instance");
	parser.addOption(singleInst);

	QCommandLineOption newInst("new-instance", "Run new instance");
	parser.addOption(newInst);

	QCommandLineOption openUrl("url", "Url to open (Like xmpp:rabbit@40k.org?message)", "url");
	parser.addOption(openUrl);

	QCommandLineOption configDir("config", "Custom config directory", "path");
	parser.addOption(configDir);

	if(!parser.parse(args)) {
		parser.showHelp(0);
		exit(0);
		return;
	}

	QString messageToServer;
#if defined(Q_OS_WIN) || (defined(Q_OS_LINUX) && !defined(Q_WS_MAEMO_5) && !defined(MEEGO_EDITION))
	bool singleInstance = !parser.isSet("new-instance");
#else
	bool singleInstance = parser.isSet("single-instance");
#endif
	
	if (singleInstance) {
		messageToServer = QLatin1String("arguments: ");
		QByteArray data;
		{
			QDataStream s(&data, QIODevice::WriteOnly);
			s << args;
		}
		messageToServer += QLatin1String(data.toBase64());
	}

	if (parser.value("url").count()) {
		messageToServer = QLatin1String("url: ");
		messageToServer += parser.value("url");
	} else if (args.count() == 2) {
		const QString &possibleUrl = args[1];
		QUrl url = QUrl::fromEncoded(possibleUrl.toUtf8());
		if (url.isValid() && !url.scheme().isEmpty())
			messageToServer = QLatin1String("url: ") + possibleUrl;
	}

	if(parser.value("config").count()) {
		Profile::instance()->setCustomProfilePath(parser.value("config"));
	}

	if (!messageToServer.isEmpty()) {
		bool shouldExit = false;
		d->initLocalPeer(messageToServer, &shouldExit);

		if (parser.positionalArguments().isEmpty()) {
			if (parser.isSet("version")) {
				printVersion();
				exit(0);
				return;
			}

			if (!parser.unknownOptionNames().isEmpty() || parser.isSet("help")) {
				parser.showHelp(0);
				exit(0);
				return;
			}
		}

		if (shouldExit) {
			exit(0);
			return;
		}
	}

#endif
	
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
	QMap<QString, QString> errors;

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
				typedef const char * (*QutimPluginVerificationFunction)();
				QutimPluginVerificationFunction verificationFunction = NULL;
#ifdef QUTIM_TEST_PERFOMANCE
				QTime timer;
				int libLoadTime, verifyTime, instanceTime, initTime;
				quint64 debugId = 0;
#endif // QUTIM_TEST_PERFOMANCE
				{
#ifdef QUTIM_TEST_PERFOMANCE
					timer.start();
#endif // QUTIM_TEST_PERFOMANCE
					QScopedPointer<QLibrary> lib(new QLibrary(filename));
					if (lib->load()) {
						errors.remove(filename);
#ifdef QUTIM_TEST_PERFOMANCE
						libLoadTime = timer.elapsed();
						timer.restart();
#endif // QUTIM_TEST_PERFOMANCE
						verificationFunction = reinterpret_cast<QutimPluginVerificationFunction>(
									lib->resolve("qutim_plugin_query_verification_data"));
#ifdef QUTIM_TEST_PERFOMANCE
						verifyTime = timer.elapsed();
						timer.restart();
#endif // QUTIM_TEST_PERFOMANCE
						if (!verificationFunction) {
							lib->unload();
							errors.insert(filename, filename + " has no valid verification data");
							continue;
						}
						QString error;
						if (!checkQutIMPluginData(verificationFunction(), &debugId, &error)) {
							lib->unload();
							errors.insert(filename, "Error while loading plugin " + filename + ": " + error);
							continue;
						}
					} else {
						errors.insert(filename, lib->errorString());
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
					if (debugId)
						debugAddPluginId(debugId, plugin->metaObject());
					plugin->init();
#ifdef QUTIM_TEST_PERFOMANCE
					initTime = timer.elapsed();
					qDebug() << files[i].fileName();
                    qDebug() << "load:" << libLoadTime << "ms, verify:" << verifyTime
							<< "ms, instance:" << instanceTime << "ms, init:" << initTime << "ms";
#endif // QUTIM_TEST_PERFOMANCE
                    if (plugin->p->validate()) {
                        PluginInfo::Data *info = plugin->p->info.data();
                        info->inited = 1;
                        info->fileName = filename;
                        QFileInfo fileInfo = info->fileName;
                        QString baseName = fileInfo.baseName();
                        if (baseName.startsWith(QStringLiteral("lib")))
                            baseName.remove(0, 3);
                        info->libraryName = baseName;

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
						errors.insert(filename, loader->errorString());
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
		foreach (const QString &error, errors)
			qDebug() << error;
	}

#ifndef NO_COMMANDS
//	{
//		QMap<QString, Plugin *> handlers;
//		QMap<QString, Plugin*>::Iterator it;
//		foreach (QPointer<Plugin> plugin, d->plugins) {
//			if (CommandArgumentsHandler *handler = qobject_cast<CommandArgumentsHandler*>(plugin.data()))
//				handlers.insert(handler->argumentHandlerName(), plugin.data());
//		}

//		if (parser.count("help") || parser.positional().size() > 1) {
//			for (; it != handlers.end(); ++it) {
//				Plugin *plugin = it.value();
//				parser.addSection(plugin->info().name() + " (" + it.key() + ")");
//			}
//			parser.showUsage();
//			exit(0);
//			return;
//		}

//		if (!parser.positional().isEmpty()) {
//			for (; it != handlers.end(); ++it) {
//				Plugin *plugin = it.value();
//				CommandArgumentsHandler *handler = qobject_cast<CommandArgumentsHandler*>(plugin);
//				foreach (const CommandArgument &argument, handler->arguments()) {
//					parser.add(argument.name(), argument.description(), argument.types(), argument.group());
//					foreach (const QString &alias, argument.aliases())
//						parser.alias(argument.name(), alias);
//				}
//			}
//		}
//	}
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
		qDebug() << QString("Found %1 for %2").arg(generator->metaObject()->className(), meta->className());
		return obj;
	}
	qWarning() << meta->className() << " extension isn't found";
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
				qWarning() << meta->className() << " has no 'Extension' class info";
				continue;
			}
			qDebug() << "Found " << meta->className() << " for " << name.constData();
			configBackends << info.generator()->generate<ConfigBackend>();
		}
	}
	QSet<PluginInfo::Data*> disabledPlugins;
	Config pluginsConfig;
	pluginsConfig.beginGroup("plugins/list");
	{
        foreach (QPointer<Plugin> plugin, d->plugins) {
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
		QVariantMap::const_iterator it = selected.constBegin();
		QSet<QString> choosedProtocols;
		for (; it != selected.constEnd(); it++) {
			QString className = it.value().toString();
			if (className == QLatin1String("none")) {
				choosedProtocols.insert(it.key());
				continue;
			}
			const ExtensionInfo info = extsHash.value(className.toLatin1());
			if (info.generator() && info.generator()->extends<Protocol>()) {
				const QMetaObject *meta = info.generator()->metaObject();
				QByteArray name = MetaObjectBuilder::info(meta, "Protocol");
				if (name.isEmpty() || name != it.key())
					continue;
				qDebug() << name << meta->className();
				Protocol *protocol = info.generator()->generate<Protocol>();
				AccountManagerPrivate::getPrivate(&d->accountManager)->addProtocol(protocol);
				choosedProtocols.insert(it.key());
				usedExtensions << meta->className();

                connect(protocol, SIGNAL(destroyed(QObject*)), this, SLOT(_q_protocolDestroyed(QObject*)));
			}
		}
		const ExtensionInfoList exts = extensions(Protocol::staticMetaObject.className()); //p->extensions;
		ExtensionInfoList::const_iterator it2 = exts.constBegin();
		for(; it2 != exts.end(); it2++) {
			const ObjectGenerator *gen = it2->generator();
			const QMetaObject *meta = gen->metaObject();
			QString name = QLatin1String(MetaObjectBuilder::info(meta, "Protocol"));
            if (name.isEmpty() || choosedProtocols.contains(name))
				continue;
			Protocol *protocol = gen->generate<Protocol>();
			AccountManagerPrivate::getPrivate(&d->accountManager)->addProtocol(protocol);
			choosedProtocols.insert(name);
			usedExtensions << meta->className();
			selected.insert(protocol->id(), QString::fromLatin1(meta->className()));

            connect(protocol, SIGNAL(destroyed(QObject*)), this, SLOT(_q_protocolDestroyed(QObject*)));
		}

		group.setValue("list", selected);
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
			qDebug() << "Startup:" << exts.at(i).generator()->metaObject()->className() << "," << timer.elapsed() << "ms";
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
		qDebug() << proto->id() << ", load:" << timer.elapsed() << "ms";
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
					qDebug() << plugin->metaObject()->className() <<  ", load:" << timer.elapsed() << "ms";
#endif
					continue;
				}
#ifdef QUTIM_TEST_PERFOMANCE
				qDebug() << plugin->metaObject()->className() << ", load:" << timer.elapsed() << "ms";
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
		qDebug() << i << d->plugins.size() << d->plugins.at(i).data()->metaObject()->className();
	}
	Event("startup").send();
}

void ModuleManager::onQuit()
{
	foreach (Account *account, d->accountManager.accounts()) {
		Status status = account->status();
		account->config().setValue("lastStatus", status);
		status.setType(Status::Offline);
		status.setChangeReason(Status::ByQuit);
		account->setUserStatus(status);
	}

	Event("aboutToQuit").send();
    foreach(QPointer<Plugin> plugin, d->plugins) {
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

#include "moc_modulemanager.cpp"

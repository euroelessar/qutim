/*****************************************************************************
	Plugin System

	Copyright (c) 2008 by m0rph <m0rph.mailbox@gmail.com>
			2008-2009 by  Rustam Chakin <qutim.develop@gmail.com>
						  Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#define QUTIM_BUILD_VERSION_MAJOR               0
#define QUTIM_BUILD_VERSION_MINOR               2
#define QUTIM_BUILD_VERSION_SECMINOR            0

#include <QtAlgorithms>
#include <QDir>
#include <QStringList>
#include <QtGlobal>
#include <QDebug>
#include <QDomDocument>
#include "pluginsystem.h"
#include "libqutim/systeminfo.h"
#include "abstractlayer.h"
#include "abstractcontextlayer.h"
#include "console.h"
#include "iconmanager.h"
#ifndef NO_CORE_ANTISPAM
#  include "corelayers/antispam/abstractantispamlayer.h"
#endif
#ifndef NO_CORE_CHAT
#  include "corelayers/chat/chatlayerclass.h"
#endif
#ifndef NO_CORE_CONTACTLIST
#  include "corelayers/contactlist/defaultcontactlist.h"
#endif
#ifndef NO_CORE_EMOTICONS
#  include "corelayers/emoticons/abstractemoticonslayer.h"
#endif
#ifndef NO_CORE_HISTORY
#  include "corelayers/history/historylayer.h"
#endif
#ifndef NO_CORE_NOTIFICATION
#  include "corelayers/notification/defaultnotificationlayer.h"
#endif
#ifndef NO_CORE_SETTINGS
#  include "corelayers/settings/qsettingslayer.h"
#endif
#ifndef NO_CORE_SOUNDENGINE
#  include "corelayers/soundengine/defaultsoundenginelayer.h"
#endif
#ifndef NO_CORE_SPELLER
#  include "corelayers/speller/spellerlayerclass.h"
#endif
#ifndef NO_CORE_STATUS
#  include "corelayers/status/defaultstatuslayer.h"
#endif
#ifndef NO_CORE_TRAY
#  include "corelayers/tray/defaulttraylayer.h"
#endif
#ifndef NO_CORE_VIDEOENGINE
//#  include "corelayers/videoengine/defaultvideoenginelayer.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <cstdio>

enum DebugType
{
	DebugConsole    = 0x01,
	DebugFile       = 0x02,
	DebugWindow     = 0x04
};
Q_DECLARE_FLAGS(DebugTypes,DebugType)
static DebugTypes debug_type;
static int &debug_int = *reinterpret_cast<int *>( &debug_type );
static std::ofstream debug_out;
static Console *debug_console = 0;

#define DEBUG_MESSAGE(Type) \
	case Qt##Type##Msg: \
		if( debug_type & DebugConsole ) \
			printf( #Type ": %s\n", msg ) ; \
		if( debug_type & DebugFile ) \
			debug_out << #Type ": " << msg << std::endl; \
		break;

void globalHandleDebug(QtMsgType type, const char *msg)
{
	switch( type )
	{
		DEBUG_MESSAGE(Debug)
		DEBUG_MESSAGE(Warning)
		DEBUG_MESSAGE(Critical)
		DEBUG_MESSAGE(Fatal)
	}
	if( debug_type & DebugFile )
		debug_out.flush() ;
	if( (debug_type & DebugWindow) && debug_console )
		debug_console->appendMsg( QString::fromLocal8Bit( msg, qstrlen( msg ) ), type );
}

QSettings::SettingsMap parseDict(const QDomNode & root_element)
{
	QSettings::SettingsMap style_hash;
	if (root_element.isNull())
		return style_hash;
	QDomNode subelement =root_element;//.firstChild();
	QString key="";
	for (QDomNode node = subelement.firstChild(); !node.isNull(); node = node.nextSibling())
	{
		QDomElement element = node.toElement();
		if(element.nodeName()=="key")
			key=element.text();
		else
		{
			QVariant value;
			if(element.nodeName()=="true")
				value=QVariant(true);
			else if(element.nodeName()=="false")
				value=QVariant(false);
			else if(element.nodeName()=="real")
				value=QVariant(element.text().toDouble());
			else if(element.nodeName()=="string")
				value=QVariant(element.text());
			else if(element.nodeName()=="integer")
				value=QVariant(element.text().toInt());
			else if(element.nodeName()=="dict")
				value = parseDict(node);
			style_hash.insert(key,value);
		}
	}
	return style_hash;
}
bool plistReadFunc(QIODevice &device, QSettings::SettingsMap &map)
{
	QDomDocument theme_dom;

	if (theme_dom.setContent(&device))
	{
		QDomElement root_element = theme_dom.documentElement();
		if (root_element.isNull())
			return false;
		map = parseDict(root_element.firstChild());
	}
	return true;
}
bool plistWriteFunc(QIODevice &device, const QSettings::SettingsMap &map)
{
	return false;
}

struct PluginSystem::EventId
{
	quint16 contact_context;
	quint16 item_added;
	quint16 item_removed;
	quint16 item_moved;
	quint16 item_icon_changed;
	quint16 item_changed_name;
	quint16 item_changed_status;
	quint16 account_is_online;
	quint16 sending_message_before_showing;
	quint16 sending_message_after_showing;
	quint16 senging_message_after_showing_last_output;
	quint16 pointers_are_initialized;
	quint16 system_notification;
	quint16 user_notification;
	quint16 receiving_message_first_level;
	quint16 receiving_message_second_level;
	quint16 receiving_message_third_level;
	quint16 receiving_message_fourth_level;
	quint16 item_typing_notification;
	quint16 item_ask_tooltip;
	quint16 item_ask_additional_info;
	quint16 all_plugins_loaded;
};

static struct AbstractArgStructure
{
	AbstractArgStructure() : argc(0), argv(0) {}
	int argc;
	char **argv;
} qutIM_args;

PluginSystem::PluginSystem()
	: m_abstract_layer(AbstractLayer::instance()), m_new_event_id(0)
{
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim", "qutimsettings");
#if defined(Q_OS_WIN)
	debug_int = settings.value("debug/type", 0).toInt();
#else
	debug_int = settings.value("debug/type", DebugConsole).toInt();
#endif
	if(debug_type & DebugFile)
	{
		QString file_name = QFileInfo(settings.fileName()).absoluteDir().filePath(settings.value("debug/file","system.log").toString());
		bool append = settings.value("debug/append",false).toBool();
		debug_out.open(file_name.toLocal8Bit(), append ? std::ios_base::app : std::ios_base::out );
	}
	debug_console = 0;
	qInstallMsgHandler(globalHandleDebug);
}

void PluginSystem::init()
{
	qRegisterMetaType<QTextCursor>("QTextCursor");
	if(debug_type & DebugWindow)
		debug_console = new Console();
	QCoreApplication::setApplicationName("qutIM");
	QCoreApplication::setApplicationVersion("0.2 beta");
	QCoreApplication::setOrganizationDomain("qutim.org");
	m_plist_format = QSettings::registerFormat("plist",plistReadFunc,plistWriteFunc);
	m_events = new EventId;
	m_events->contact_context = registerEventHandler("Core/ContactList/ContactContext");
	m_events->item_added = registerEventHandler("Core/ContactList/ItemAdded");
	m_events->item_removed = registerEventHandler("Core/ContactList/ItemRemoved");
	m_events->item_moved = registerEventHandler("Core/ContactList/ItemMoved");
	m_events->item_icon_changed = registerEventHandler("Core/ContactList/ItemIconChanged");
	m_events->item_changed_name = registerEventHandler("Core/ContactList/ItemChangedName");
	m_events->item_changed_status = registerEventHandler("Core/ContactList/ItemChangedStatus");
	m_events->account_is_online = registerEventHandler("Core/Protocol/AccountIsOnline");
	m_events->sending_message_before_showing = registerEventHandler("Core/ChatWindow/SendLevel1");
	m_events->sending_message_after_showing = registerEventHandler("Core/ChatWindow/SendLevel2");
	m_events->senging_message_after_showing_last_output = registerEventHandler("Core/ChatWindow/SendLevel3");
	m_events->pointers_are_initialized = registerEventHandler("Core/Layers/Initialized");
	m_events->system_notification = registerEventHandler("Core/Notification/System");
	m_events->user_notification = registerEventHandler("Core/Notification/User");
	m_events->receiving_message_first_level = registerEventHandler("Core/ChatWindow/ReceiveLevel1");
	m_events->receiving_message_second_level = registerEventHandler("Core/ChatWindow/ReceiveLevel2");
	m_events->receiving_message_third_level = registerEventHandler("Core/ChatWindow/ReceiveLevel3");
	m_events->receiving_message_fourth_level = registerEventHandler("Core/ChatWindow/ReceiveLevel4");
	m_events->item_typing_notification = registerEventHandler("Core/Notification/Typing");
	m_events->item_ask_tooltip = registerEventHandler("Core/ContactList/AskTooltip");
	m_events->item_ask_additional_info = registerEventHandler("Core/ContactList/AskItemAdditionalInfo");
	m_events->all_plugins_loaded = registerEventHandler("Core/AllPluginsLoaded");

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim", "qutimsettings");

	QDir dir = qApp->applicationDirPath();
	QDir::setCurrent( qApp->applicationDirPath() );
	QDir current_dir = QDir::current();
	QString path = dir.absolutePath();
	// 1. Windows, ./
	m_share_paths << current_dir.relativeFilePath( path );
	// 2. MacOS X, ../Resources
	dir.cdUp();
#if defined(Q_WS_MAC)
	path = dir.absolutePath();
	path += QDir::separator();
	path += "Resources";
	m_share_paths << current_dir.relativeFilePath( path );
#endif
	// 3. Unix, ../share/qutim
	path = dir.absolutePath();
	path += QDir::separator();
	path += "share";
	path += QDir::separator();
	path += "qutim";
	m_share_paths << current_dir.relativeFilePath( path );
	// 4. Safe way, ~/.config/qutim
	QFileInfo config_dir = settings.fileName();
	dir = qApp->applicationDirPath();
	if( config_dir.canonicalPath().contains( dir.canonicalPath() ) )
		m_share_paths << current_dir.relativeFilePath( config_dir.absolutePath() );
	else
		m_share_paths << config_dir.absolutePath();
#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
	m_share_paths.removeDuplicates();
#else
	m_share_paths = QSet<QString>().fromList(m_share_paths).toList();
#endif
	qDebug() << m_share_paths;
}

PluginSystem::~PluginSystem()
{
	delete m_events;
}


PluginSystem &PluginSystem::instance()
{
	static PluginSystem ps;
	return ps;
}

void PluginSystem::aboutToQuit()
{
	foreach(LayerInterface *layer_interface, m_layer_interfaces.values())
	{
		if(layer_interface) layer_interface->release();
	}
	foreach(PluginInterface *plugin, m_plugins)
	{
		if(plugin) plugin->release();
	}
	foreach(ProtocolInterface *protocol, m_protocols)
	{
		if(protocol) protocol->release();
	}
	debug_out.close();
	delete debug_console;
	debug_console = 0;
}

PluginInterface *PluginSystem::loadPlugin(QObject *instance)
{
	PluginInterface *plugin = qobject_cast<PluginInterface *>(instance);
	if( !plugin )
	{
		delete instance;
		return 0;
	}

	if( !plugin->init(this) )
	{
		delete instance;
		return 0;
	}

	if( plugin->type() == "protocol" )
	{
		if( ProtocolInterface *protocol = plugin_cast<ProtocolInterface *>(instance) )
		{
			m_protocols.insert (protocol->name(), instance);
			qDebug( "Protocol \"%s\" was loaded", qPrintable(plugin->name()) );
		}
		else
		{
			qDebug( "Protocol \"%s\" was not loaded", qPrintable(plugin->name()) );
			plugin->release();
			delete instance;
			return 0;
		}
	}
	else
	{
		m_plugins.append(instance);
		if( plugin_cast<LayerPluginInterface *>(instance) && plugin->type() == "layer" )
			qDebug( "Layer plugin \"%s\" was loaded", qPrintable(plugin->name()) );
		else if( plugin_cast<DeprecatedSimplePluginInterface *>(instance) )
			qDebug( "Deprecated simple plugin \"%s\" was loaded", qPrintable(plugin->name()) );
		else if( plugin_cast<SimplePluginInterface *>(instance) )
			qDebug( "Simple plugin \"%s\" was loaded", qPrintable(plugin->name()) );
		else
			qDebug( "Unknown plugin \"%s\" was loaded", qPrintable(plugin->name()) );
	}
	return plugin;
}


bool PluginSystem::unloadPlugin(const QString &name)
{
	for(int i = 0; i < m_plugins.count(); ++i)
	{
		PluginInterface *plugin = m_plugins[i];
		if( plugin && plugin->name() == name )
		{
			plugin->release();
			delete plugin;
			m_plugins.removeAt(i);
			return true;
		}
	}
	return false;
}


bool PluginSystem::unloadPlugin(PluginInterface *object)
{
	if(!object)
		return false;
	for(int i = 0; i < m_plugins.count(); ++i)
	{
		PluginInterface *plugin = m_plugins[i];
		if( plugin == object )
		{
			plugin->release();
			delete plugin;
			m_plugins.removeAt(i);
			return true;
		}
	}
	return false;
}


int PluginSystem::pluginsCount()
{
	return m_plugins.count();
}


SimplePluginInterface *PluginSystem::getPluginByIndex(int index)
{
	//assert (!m_loaders.empty());
	//assert (0 <= index && index < m_loaders.count());
	SimplePluginInterface *plugin = plugin_cast<SimplePluginInterface *>(m_plugins[index]);
	if( plugin )
		return plugin;
	return plugin_cast<DeprecatedSimplePluginInterface *>(m_plugins[index]);
}


void PluginSystem::registerEventHandler(const EventType &type, DeprecatedSimplePluginInterface *plugin)
{
	m_registered_events_plugins.insert(type, plugin);
}


void PluginSystem::releaseEventHandler(const QString &event_id, PluginInterface *plugin)
{
//	PluginEventLists::iterator it;
//	it = m_event_lists.find(event_id);
//	if(it == m_event_lists.end())
//		return;
//
//	SimplePluginList *handlers_list = &it.value();
//	int i = handlers_list->indexOf(plugin);
//	if(i == -1)
//		return;
//
//	handlers_list->removeAt(i);
//	if(handlers_list->isEmpty())
//		m_event_lists.erase(it);
}


void PluginSystem::processEvent(PluginEvent &event)
{
//	PluginEventLists::iterator it;
//	it = m_event_lists.find(event.id);
//	if(it == m_event_lists.end())
//		return;
//
//	SimplePluginList *handlers_list = &m_event_lists[event.id];
//
//	foreach(SimplePluginInterdace *plugin, (*handlers_list))
//	{
//		plugin->processEvent(event);
//	}
}

QObjectList PluginSystem::findPlugins(const QString &path, const QStringList &paths)
{
	static QSet<QString> plugin_paths_list;
	QObjectList result;
	QDir plugins_dir = path;
#ifdef Q_OS_WIN32
	qDebug("Search plugins at: \"%s\"", qPrintable(path));
	qDebug("Files: \"%s\"", qPrintable(plugins_dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot).join("\", \"")));
#endif
//	// We should load only libraries
//#ifdef Q_OS_WIN
//	QStringList filters = QStringList() << "*.dll";
//#elif defined(Q_OS_MAC)
//	QStringList filters = QStringList() << "*.dylib";
//#elif defined(Q_OS_UNIX)
//	QStringList filters = QStringList() << "*.so";
//#else
//	QStringList filters = QStringList() << "*.*";
//#endif
	QFileInfoList files = plugins_dir.entryInfoList(QDir::AllEntries);

	for(int i = 0; i < files.count(); ++i)
	{
		QString filename = files[i].canonicalFilePath();
		if(plugin_paths_list.contains(filename) || !QLibrary::isLibrary(filename) || !files[i].isFile())
			continue;
		plugin_paths_list << filename;
		qDebug() << filename;
		QPluginLoader *loader = new QPluginLoader(filename);
		QObject *object = loader->instance();
		PluginInterface *plugin = qobject_cast<PluginInterface *>(object);
		if( CmdArgsHandler *cmdhandler = qobject_cast<CmdArgsHandler *>( object ) )
			cmdhandler->setCmdArgs( qutIM_args.argc, qutIM_args.argv );
		if(plugin)
		{
//			PluginInfo info = { plugin->name(), plugin->description(), plugin->type(), QIcon() };
			result << object;
			PluginContainerInterface *container = plugin_cast<PluginContainerInterface *>(object);
			if( container )
			{
				QObjectList plugins = container->loadPlugins(paths);
				while( !plugins.isEmpty() )
					result << plugins.takeFirst();
			}
		}
		else
		{
			qWarning("Error while loading plugin %s: %s", qPrintable(filename), qPrintable(loader->errorString()));
			loader->unload();
		}
		delete loader;
	}
	return result;
}

QObjectList PluginSystem::loadPlugins(int argc, char *argv[])
{
	qutIM_args.argc = argc;
	qutIM_args.argv = argv;
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim", "qutimsettings");
	
	QObjectList plugins;
	QStringList paths;
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
	while(config_paths.size())
		paths << config_paths.takeFirst();
	qDebug() << paths;
	foreach(const QString &path, paths)
		plugins << findPlugins(path, paths);
	return plugins;
}

void PluginSystem::initPlugins( QObjectList plugins )
{
	foreach(QObject *plugin, plugins)
		loadPlugin(plugin);

#ifndef NO_CORE_SETTINGS
	if( !m_layer_interfaces.contains( SettingsLayer ) )
	{
		LayerInterface *layer_interface = new QSettingsLayer();
		layer_interface->init(this);
		m_layer_interfaces.insert(SettingsLayer, layer_interface);
	}
#endif
	m_settings = static_cast<SettingsLayerInterface *>( m_layer_interfaces.value( SettingsLayer ) );
}


PluginInfoList PluginSystem::getPluginsByType(const QString &type)
{
	PluginInfoList temp;
	if ( type != "protocol")
	{
		for(int i = 0; i < pluginsCount(); ++i)
		{
			PluginInterface *plugin = getPluginByIndex(i);
			if( !plugin )
				continue;
			if(plugin->type() == type)
			{
				PluginInfo info;
				info.name = plugin->name();
				info.description = plugin->description();
				info.type = plugin->type();
				if(plugin->icon())
					info.icon = *plugin->icon();
	
				temp.append(info);
			}
		}
	}
	else
	{
		foreach(ProtocolInterface *plugin, m_protocols.values() )
		{
			if( !plugin )
				continue;
			PluginInfo info;
			info.name = plugin->name();
			info.description = plugin->description();
			info.type = plugin->type();
			if(plugin->icon())
				info.icon = *plugin->icon();
			temp.append(info);
		}
	}
	
	return temp;
}

bool PluginSystem::setLayerInterface( LayerType type, LayerInterface *layer_interface)
{
	if(type<0 || type>=InvalidLayer)
		type = InvalidLayer;

	if(type == InvalidLayer || m_layer_interfaces.contains(type) || !layer_interface)
		return false;

	if(layer_interface->init(this))
	{
		m_layer_interfaces.insert(type, layer_interface);
		return true;
	}
	return false;
}

LayerInterface *PluginSystem::getLayerInterface(LayerType type)
{
	return m_layer_interfaces.value(type, 0);
}

void PluginSystem::initiateLayers(const QString &profile_name)
{
#ifndef NO_CORE_ANTISPAM
	if(!m_layer_interfaces.contains(AntiSpamLayer))
	{
		LayerInterface *layer_interface = new AbstractAntiSpamLayer();
		layer_interface->init(this);
		m_layer_interfaces.insert(AntiSpamLayer, layer_interface);
	}
#endif
#ifndef NO_CORE_CHAT
	if(!m_layer_interfaces.contains(ChatLayer))
	{
		LayerInterface *layer_interface = new ChatLayerClass();
		layer_interface->init(this);
		m_layer_interfaces.insert(ChatLayer, layer_interface);
	}
#endif
#ifndef NO_CORE_CONTACTLIST
	if(!m_layer_interfaces.contains(ContactListLayer))
	{
		LayerInterface *layer_interface = new DefaultContactList();
		layer_interface->init(this);
		m_layer_interfaces.insert(ContactListLayer, layer_interface);
	}
#endif
#ifndef NO_CORE_EMOTICONS
	if(!m_layer_interfaces.contains(EmoticonsLayer))
	{
		LayerInterface *layer_interface = new CoreEmoticons::AbstractEmoticonsLayer();
		layer_interface->init(this);
		m_layer_interfaces.insert(EmoticonsLayer, layer_interface);
	}
#endif
#ifndef NO_CORE_HISTORY
	if(!m_layer_interfaces.contains(HistoryLayer))
	{
		LayerInterface *layer_interface = new AbstractHistoryLayer();
		layer_interface->init(this);
		m_layer_interfaces.insert(HistoryLayer, layer_interface);
	}
#endif
#ifndef NO_CORE_NOTIFICATION
	if(!m_layer_interfaces.contains(NotificationLayer))
	{
		LayerInterface *layer_interface = new DefaultNotificationLayer();
		layer_interface->init(this);
		m_layer_interfaces.insert(NotificationLayer, layer_interface);
	}
#endif
#ifndef NO_CORE_SOUNDENGINE
	if(!m_layer_interfaces.contains(SoundEngineLayer))
	{
		LayerInterface *layer_interface = new DefaultSoundEngineLayer();
		layer_interface->init(this);
		m_layer_interfaces.insert(SoundEngineLayer, layer_interface);
	}
#endif
#ifndef NO_CORE_VIDEOENGINE
//	if(!m_layer_interfaces.contains(VideoEngineLayer))
//	{
//		LayerInterface *layer_interface = new DefaultVideoEngineLayer();
//		layer_interface->init(this);
//		m_layer_interfaces.insert(SoundVideoLayer, layer_interface);
//	}
#endif
#ifndef NO_CORE_SPELLER
	if(!m_layer_interfaces.contains(SpellerLayer))
	{
		LayerInterface *layer_interface = new SpellerLayerClass();
		layer_interface->init(this);
		m_layer_interfaces.insert(SpellerLayer, layer_interface);
	}
#endif
#ifndef NO_CORE_STATUS
	if(!m_layer_interfaces.contains(StatusLayer))
	{
		LayerInterface *layer_interface = new DefaultStatusLayer();
		layer_interface->init(this);
		m_layer_interfaces.insert(StatusLayer, layer_interface);
	}
#endif
#ifndef NO_CORE_TRAY
	if(!m_layer_interfaces.contains(TrayLayer))
	{
		LayerInterface *layer_interface = new DefaultTrayLayer();
		layer_interface->init(this);
		m_layer_interfaces.insert(TrayLayer, layer_interface);
	}
#endif



//	AbstractContactList::instance().setLayerInterface(m_layer_interfaces.value(ContactListLayer));
//	AbstractNotificationLayer::instance().setLayerInterface(m_layer_interfaces.value(NotificationLayer));

	for(int type = 0;type<InvalidLayer;type++)
	{
		LayerInterface *interface = m_layer_interfaces.value(static_cast<LayerType>(type), 0);
		AbstractLayer::instance().setLayerInterface(static_cast<LayerType>(type), interface);
		if(interface)
		{
			for(int type2 = 0;type2<InvalidLayer;type2++)
			{
				LayerInterface *interface2 = m_layer_interfaces.value(static_cast<LayerType>(type2), 0);
				if(interface2 && type2 != type)
					interface->setLayerInterface(static_cast<LayerType>(type2), interface2);
			}
		}
	}
	for(int type = 0;type<InvalidLayer;type++)
	{
		LayerInterface *interface = m_layer_interfaces.value(static_cast<LayerType>(type), 0);
		if(interface)
			interface->setProfileName(profile_name);
	}

	Event event(m_events->all_plugins_loaded, 0);
	sendEvent(event);
}

QWidget *PluginSystem::getLoginWidget(const QString &protocol_name)
{
		ProtocolInterface *protocol = m_protocols.value(protocol_name);
		if(!protocol)
			return 0;

		return protocol->loginWidget();
}

void PluginSystem::removeLoginWidgetByName(const QString &protocol_name)
{
		ProtocolInterface *protocol = m_protocols.value(protocol_name);
		if(!protocol)
			return;

		protocol->removeLoginWidget();
}

void PluginSystem::removeAccount(const QString &protocol_name, const QString &account_name)
{
		ProtocolInterface *protocol = m_protocols.value(protocol_name);
		if(!protocol)
			return;

		protocol->removeAccount(account_name);
}

void PluginSystem::loadProfile(const QString &profile_name)
{
	m_profile_name = profile_name;
	SystemsCity::instance().setProfileName( profile_name );
//	loadPlugins();
	initiateLayers(profile_name);
	QList<PluginInterface *> all_plugins;
	foreach(PluginInterface *plugin, m_plugins)
		all_plugins << plugin;
	foreach(ProtocolInterface *protocol, m_protocols)
		all_plugins << protocol;

	foreach(PluginInterface *plugin, all_plugins)
	{
		if(plugin)
		{
			qDebug( "\"%s\"", qPrintable(plugin->name()) );
			plugin->setProfileName(m_profile_name);
		}
	}
	IconInfoList pack;
	foreach(PluginInterface *plugin, all_plugins)
	{
		if(plugin)
		{
			QIcon *icon = plugin->icon();
			if(icon)
			{
				IconInfo info;
				info.name = plugin->name().toLower();
				info.category = IconInfo::Plugin;
				info.icon = *icon;
				pack << info;
			}
		}
	}
	IconManager::instance().addPack("plugins", pack);
}

void PluginSystem::applySettingsPressed(const QString &protocol_name)
{
	ProtocolInterface *protocol = m_protocols.value(protocol_name);
	if(!protocol)
		return;

	protocol->applySettingsPressed();
}

QList<QMenu*> PluginSystem::addStatusMenuToTrayMenu()
{
	QList<QMenu*> status_menu_list;
	foreach(ProtocolInterface *protocol, m_protocols)
	{
					if(protocol)
					{
			foreach(QMenu *account_menu, protocol->getAccountStatusMenu())
			{
				status_menu_list.append(account_menu);
			}
					}
	}
	return status_menu_list;
}

void PluginSystem::addAccountButtonsToLayout(QHBoxLayout *account_buttons_layout)
{
//	if(m_layer_interfaces.contains(ContactListLayer))
//		reinterpret_cast<ContactListLayerInterface *>(m_layer_interfaces.value(ContactListLayer))->setAccountButtonsLayout(account_buttons_layout);
	foreach(ProtocolInterface *protocol, m_protocols)
	{
		if(protocol)
		{
			protocol->addAccountButtonsToLayout(account_buttons_layout);
		}
	}
	Q_REGISTER_EVENT(account_buttons, "Core/ContactList/AccountButtonsLayout");
	Event(account_buttons, 1, &account_buttons_layout).send();
}

void PluginSystem::saveLoginDataByName(const QString &protocol_name)
{
		ProtocolInterface *protocol = m_protocols.value(protocol_name);
		if(!protocol)
			return;
		protocol->saveLoginDataFromLoginWidget();
}

QList<SettingsStructure> PluginSystem::getSettingsByName(const QString &protocol_name) const
{
		ProtocolInterface *protocol = m_protocols.value(protocol_name);
		if(!protocol)
		{
			QList<SettingsStructure> list;
			return list;
		}
		return protocol->getSettingsList();
}

void PluginSystem::removeProtocolSettingsByName(const QString &protocol_name)
{
		ProtocolInterface *protocol = m_protocols.value(protocol_name);
		if(!protocol)
			return;
		protocol->removeProtocolSettings();
}

QList<AccountStructure> PluginSystem::getAccountsList() const
{
	QList<AccountStructure> full_account_list;
	foreach(ProtocolInterface *protocol, m_protocols)
	{

		if(protocol)
		{
			foreach(AccountStructure account_structure, protocol->getAccountList())
			{
				full_account_list.append(account_structure);
			}
		}
	}
	return full_account_list;
}

QList<AccountStructure> PluginSystem::getAccountsStatusesList() const
{
	QList<AccountStructure> full_account_list;
	foreach(ProtocolInterface *protocol, m_protocols)
	{

					if(protocol)
					{

			foreach(AccountStructure account_structure, protocol->getAccountStatuses())
			{
				full_account_list.append(account_structure);
			}
					}
	}
	return full_account_list;
}

void PluginSystem::updateStatusIcons()
{
	AbstractLayer &as = AbstractLayer::instance();
	as.updateTrayIcon();
}

void PluginSystem::setAutoAway()
{
	foreach(ProtocolInterface *protocol, m_protocols)
	{

					if(protocol)
					{
			protocol->setAutoAway();
					}
	}
}

void PluginSystem::setStatusAfterAutoAway()
{
	foreach(ProtocolInterface *protocol, m_protocols)
	{

					if(protocol)
					{
			protocol->setStatusAfterAutoAway();
					}
	}
}

bool PluginSystem::addItemToContactList(const TreeModelItem &item, const QString &name)
{
	bool result = true;
	if(item.m_item_type < 3)
		m_abstract_layer.contactList()->addItem(item, name);

	//TODO:GET THIS EVENT FOR HISTORY LAYER!
	//AbstractHistoryLayer::instance().setContactHistoryName(item);
	Event event(m_events->item_added, 2, &item, &name);
	sendEvent(event);
//	if(result)
//	{
//		foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(ItemAddedAction))
//		{
//			PluginEvent event;
//			event.system_event_type = ItemAddedAction;
//			event.args.append(&item);
//			event.args.append(&name);
//			plugin->processEvent(event);
//		}
//	}
	return result;
}

bool PluginSystem::removeItemFromContactList(const TreeModelItem &item)
{
	bool result = false;
	if(item.m_item_type < 3)
		m_abstract_layer.contactList()->removeItem(item);
	Event event(m_events->item_removed, 1, &item);
	sendEvent(event);
//	if(result)
//	{
//		foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(ItemRemovedAction))
//		{
//			PluginEvent event;
//			event.system_event_type = ItemRemovedAction;
//			event.args.append(&item);
//			plugin->processEvent(event);
//		}
//	}
	return result;
}

bool PluginSystem::moveItemInContactList(const TreeModelItem &old_item, const TreeModelItem &new_item)
{
	bool result = true;
	m_abstract_layer.contactList()->moveItem(old_item, new_item);
	Event event(m_events->item_moved, 2, &old_item, &new_item);
	sendEvent(event);
//	if(result)
//	{
//		foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(ItemMovedAction))
//		{
//			PluginEvent event;
//			event.system_event_type = ItemMovedAction;
//			event.args.append(&old_item);
//			event.args.append(&new_item);
//			plugin->processEvent(event);
//		}
//	}
	return result;
}

bool PluginSystem::setContactItemName(const TreeModelItem &item, const QString &name)
{
	bool result = true;
	if(item.m_item_type < 3)
		m_abstract_layer.contactList()->setItemName(item, name);
	Event event(m_events->item_changed_name, 2, &item, &name);
	sendEvent(event);
//	if(result)
//	{
//		foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(ItemChangedNameAction))
//		{
//			PluginEvent event;
//			event.system_event_type = ItemChangedNameAction;
//			event.args.append(&item);
//			event.args.append(&name);
//			plugin->processEvent(event);
//		}
//	}
	return result;
}

bool PluginSystem::setContactItemIcon(const TreeModelItem &item, const QIcon &icon, int position)
{
	bool result = true;
	if(item.m_item_type < 3)
		m_abstract_layer.contactList()->setItemIcon(item, icon, position);
	Event event(m_events->item_icon_changed, 3, &item, &icon, &position);
	sendEvent(event);
	return result;
}

bool PluginSystem::setContactItemRow(const TreeModelItem &item, const QList<QVariant> &row, int position)
{
	Q_UNUSED(position);
	bool result = true;
	if(item.m_item_type < 3)
		m_abstract_layer.contactList()->setItemText(item, row.toVector());
	return result;
}

bool PluginSystem::setContactItemStatus(const TreeModelItem &item, const QIcon &icon, const QString &text, int mass)
{
	bool result = true;
	if(item.m_item_type < 3)
		m_abstract_layer.contactList()->setItemStatus(item, icon, text, mass);
	Event event(m_events->item_changed_status, 4, &item, &icon, &text, &mass);
	sendEvent(event);
//	if(result)
//	{
//		foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(ItemChangedStatusAction))
//		{
//			PluginEvent event;
//			event.system_event_type = ItemChangedStatusAction;
//			event.args.append(&item);
//			event.args.append(&icon);
//			event.args.append(&text);
//			event.args.append(&mass);
//			plugin->processEvent(event);
//		}
//	}
	return result;
}

void PluginSystem::itemActivated(const TreeModelItem &item)
{
	ProtocolInterface *protocol = m_protocols.value(item.m_protocol_name);
	if(!protocol)
		return;
	protocol->itemActivated(item.m_account_name, item.m_item_name);
}

void PluginSystem::itemContextMenu(const QList<QAction*> &action_list,
		const TreeModelItem &item, const QPoint &menu_point)
{
	setCurrentContextItemToPlugins(item);
	ProtocolInterface *protocol = m_protocols.value(item.m_protocol_name);
	if(!protocol)
		return;
	protocol->itemContextMenu(action_list,item.m_account_name, item.m_item_name, item.m_item_type, menu_point);
}

bool PluginSystem::setStatusMessage(QString &status_message, bool &dshow)
{
	return m_abstract_layer.status()->setStatusMessage(status_message, dshow);
}

void PluginSystem::sendMessageToContact(const TreeModelItem &item, QString &message, int message_icon_position)
{
		
	TreeModelItem tmp_item = item;
	Event event(m_events->sending_message_after_showing, 2, &item, &message);
	sendEvent(event);
	foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(SendingMessageAfterShowing))
	{
		PluginEvent event;
		event.system_event_type = SendingMessageAfterShowing;
		QList<void *> params;
		params.append(&tmp_item);
		params.append(&message);
		event.args = params;
		plugin->processEvent(event);
	}
	event.id = m_events->senging_message_after_showing_last_output;
	sendEvent(event);
	foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(SengingMessageAfterShowingLastOutput))
	{
		PluginEvent event;
		event.system_event_type = SengingMessageAfterShowingLastOutput;
		QList<void *> params;
		params.append(&tmp_item);
		params.append(&message);
		event.args = params;
		plugin->processEvent(event);
	}
	ProtocolInterface *protocol = m_protocols.value(item.m_protocol_name);
	if(!protocol)
		return;
	if ( item.m_item_type != 32 )
	    protocol->sendMessageTo(item.m_account_name, item.m_item_name, item.m_item_type, message, message_icon_position);
	else {
	    protocol->sendMessageToConference(item.m_item_name, item.m_account_name, message);
	}
}

void PluginSystem::addMessageFromContact(const TreeModelItem &item, const QString &message
		, const QDateTime &message_date)
{
	QString tmp_message = message;
	bool stop_on_this_level = false;
	TreeModelItem tmp_item = item;
		Event event(m_events->receiving_message_first_level, 2, &tmp_item, &message);
		sendEvent(event);
	foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(ReceivingMessageFirstLevel))
	{
		PluginEvent event;
		event.system_event_type = ReceivingMessageFirstLevel;
		QList<void *> params;
		params.append(&tmp_item);
		params.append(&tmp_message);
		event.args = params;
		plugin->processEvent(event);
	}
		event.id = m_events->receiving_message_second_level;
		event.args.append(&stop_on_this_level);
		sendEvent(event);
		if(stop_on_this_level)
			return;
	foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(ReceivingMessageSecondLevel))
	{
		PluginEvent event;
		event.system_event_type = ReceivingMessageSecondLevel;
		QList<void *> params;
		params.append(&tmp_item);
		params.append(&tmp_message);
		params.append(&stop_on_this_level);
		event.args = params;
		plugin->processEvent(event);
		if ( stop_on_this_level )
			return;
	}


    if(AbstractLayer::Chat())
	AbstractLayer::Chat()->newMessageArrivedTo(item,message, message_date, false,true);
	//TODO: remove it later

	/*AbstractChatLayer &acl = AbstractChatLayer::instance();
	acl.addModifiedMessage(item, tmp_message, true, message_date);*/
}

void PluginSystem::addServiceMessage(const TreeModelItem &item, const QString &message)
{
	    if(AbstractLayer::Chat())
	AbstractLayer::Chat()->newServiceMessageArriveTo(item,message);

    /*AbstractChatLayer &acl = AbstractChatLayer::instance();
	acl.addServiceMessage(item, message);*/
}

QStringList PluginSystem::getAdditionalInfoAboutContact(const TreeModelItem &item)
{
	ProtocolInterface *protocol = m_protocols.value(item.m_protocol_name);
	if(!protocol)
	{
		QStringList additional_info;
		Event ev(m_events->item_ask_additional_info, 2, &item, &additional_info);
		sendEvent(ev);
		return additional_info;
	}
	return protocol->getAdditionalInfoAboutContact(item.m_account_name, item.m_item_name, item.m_item_type);
}

void PluginSystem::showContactInformation(const TreeModelItem &item)
{
	ProtocolInterface *protocol = m_protocols.value(item.m_protocol_name);
	if(!protocol)
		return;
	return protocol->showContactInformation(item.m_account_name, item.m_item_name, item.m_item_type);
}

void PluginSystem::sendImageTo(const TreeModelItem &item, const QByteArray &image_raw)
{
	ProtocolInterface *protocol = m_protocols.value(item.m_protocol_name);
	if(!protocol)
		return;
	return protocol->sendImageTo(item.m_account_name, item.m_item_name, item.m_item_type, image_raw);
}

void PluginSystem::addImage(const TreeModelItem &item, const QByteArray &image_raw)
{
	AbstractLayer::Chat()->addImage(item, image_raw);
}

void PluginSystem::sendFileTo(const TreeModelItem &item, const QStringList &file_names)
{
	ProtocolInterface *protocol = m_protocols.value(item.m_protocol_name);
	if(!protocol)
		return;
	return protocol->sendFileTo(item.m_account_name, item.m_item_name, item.m_item_type, file_names);
}

void PluginSystem::sendTypingNotification(const TreeModelItem &item, int notification_type)
{
	ProtocolInterface *protocol = m_protocols.value(item.m_protocol_name);
	if(!protocol)
		return;
	return protocol->sendTypingNotification(item.m_account_name, item.m_item_name, item.m_item_type, notification_type);
}

void PluginSystem::contactTyping(const TreeModelItem &item, bool typing)
{
	Event(m_events->item_typing_notification, 2, &item, &typing);

	if(AbstractLayer::Chat())
	    AbstractLayer::Chat()->setItemTypingState(item,(TypingAttribute)typing);

	/*AbstractChatLayer &acl = AbstractChatLayer::instance();
	acl.contactTyping(item, typing);*/
	AbstractLayer::ContactList()->setItemAttribute(item, ItemIsTyping, typing);
//	AbstractContactList &ac = AbstractContactList::instance();
//	ac.setItemIsTyping(item,typing);
}

void PluginSystem::messageDelievered(const TreeModelItem &item, int message_position)
{

    	if(AbstractLayer::Chat())
	    AbstractLayer::Chat()->messageDelievered(item,message_position);
    //AbstractChatLayer &acl = AbstractChatLayer::instance();
	//acl.messageDelievered(item, message_position);
}

bool PluginSystem::checkForMessageValidation(const TreeModelItem &item, const QString &message,
		int message_type, bool special_status)
{
	return AbstractLayer::AntiSpam()->checkForMessageValidation(item, message, static_cast<MessageType>(message_type), special_status);
}

void PluginSystem::notifyAboutBirthDay(const TreeModelItem &item)
{
	AbstractLayer::Notification()->userMessage(item, "", NotifyBirthday);
}

void PluginSystem::systemNotification(const TreeModelItem &item, const QString &message)
{
	AbstractLayer::Notification()->systemMessage(item, message);
	TreeModelItem tmp_item = item;
	QString tmp_message = message;
	Event event(m_events->system_notification, 2, &tmp_item, &tmp_message);
	sendEvent(event);
	foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(SystemNotification))
	{
		PluginEvent event;
		event.system_event_type = SystemNotification;
		event.args.append(&tmp_item);
		event.args.append(&tmp_message);
		plugin->processEvent(event);
	}
}

void PluginSystem::userNotification(TreeModelItem item, QString message, int type)
{
	Event event(m_events->user_notification, 3, &item, &message, &type);
	sendEvent(event);
	foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(UserNotification))
	{
		PluginEvent event;
		event.system_event_type = UserNotification;
		event.args.append(&item);
		event.args.append(&message);
		event.args.append(&type);
		plugin->processEvent(event);
	}
}

void PluginSystem::customNotification(const TreeModelItem &item, const QString &message)
{
	m_abstract_layer.notification()->userMessage(item, message, NotifyCustom);
}
QString PluginSystem::getIconFileName(const QString & icon_name)
{
	return IconManager::instance().getIconFileName(icon_name);
}
QIcon PluginSystem::getIcon(const QString & icon_name)
{
	return IconManager::instance().getIcon(icon_name);	
}
QString PluginSystem::getStatusIconFileName(const QString & icon_name, const QString & default_path)
{
	return IconManager::instance().getStatusIconFileName(icon_name, default_path);	
}
QIcon PluginSystem::getStatusIcon(const QString & icon_name, const QString & default_path)
{
	return IconManager::instance().getStatusIcon(icon_name, default_path);	
}

void PluginSystem::moveItemSignalFromCL(const TreeModelItem &old_item, const TreeModelItem &new_item)
{
	ProtocolInterface *protocol = m_protocols.value(old_item.m_protocol_name);
	if(!protocol)
		return;

	protocol->moveItemSignalFromCL(old_item, new_item);
}

QString PluginSystem::getItemToolTip(const TreeModelItem &item)
{
	ProtocolInterface *protocol = m_protocols.value(item.m_protocol_name);
	if(!protocol)
	{
		QString tooltip = item.m_item_name;
		Event e(m_events->item_ask_tooltip, 2, &item, &tooltip);
		sendEvent(e);
		return tooltip;
	}

	return protocol->getItemToolTip(item.m_account_name, item.m_item_name);
}

void PluginSystem::deleteItemSignalFromCL(const TreeModelItem& item)
{
	ProtocolInterface *protocol = m_protocols.value(item.m_protocol_name);
	if(!protocol)
		return;

	protocol->deleteItemSignalFromCL(item.m_account_name,
			item.m_item_name, item.m_item_type);
}

void PluginSystem::setAccountIsOnline(const TreeModelItem &item, bool online)
{
	QString status = online?"online":"offline";
	setContactItemStatus(item, IconManager::instance().getStatusIcon(status, item.m_protocol_name),
												status, online?0:1000);
//		AbstractContactList::instance().setAccountIsOnline(item, online);
	TreeModelItem tmp_item = item;
	Event(m_events->account_is_online, 2, &tmp_item, &online).send();
	foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(AccountIsOnlineAction))
	{
		PluginEvent event;
		event.system_event_type = AccountIsOnlineAction;
		event.args.append(&tmp_item);
		event.args.append(&online);
		plugin->processEvent(event);
	}
}

void PluginSystem::createChat(const TreeModelItem &item)
{
	Q_REGISTER_EVENT(create_event, "Core/ChatWindow/CreateChat");
	bool create = true;
	TreeModelItem tmp_item = item;
	Event(create_event, 2, &tmp_item, &create).send();
	if(AbstractLayer::Chat() && create)
			AbstractLayer::Chat()->createChat(tmp_item);
    //AbstractChatLayer::instance().createChat(item);
}

void PluginSystem::getQutimVersion(quint8 &major, quint8 &minor, quint8 &secminor, quint16 &svn)
{
	major = QUTIM_BUILD_VERSION_MAJOR;
	minor = QUTIM_BUILD_VERSION_MINOR;
	secminor = QUTIM_BUILD_VERSION_SECMINOR;
	svn = QUTIM_SVN_REVISION;
}

void PluginSystem::chatWindowOpened(const TreeModelItem &item)
{
	ProtocolInterface *protocol = m_protocols.value(item.m_protocol_name);
	if(!protocol)
		return;

	protocol->chatWindowOpened(item.m_account_name, item.m_item_name);
}

void PluginSystem::chatWindowAboutToBeOpened(const TreeModelItem &item)
{
	ProtocolInterface *protocol = m_protocols.value(item.m_protocol_name);
	if(!protocol)
		return;

	protocol->chatWindowAboutToBeOpened(item.m_account_name, item.m_item_name);
}

void PluginSystem::chatWindowClosed(const TreeModelItem &item)
{
	if ( item.m_item_type == 32 )
    {

leaveConference(item.m_protocol_name,
		item.m_item_name, item.m_account_name);
	} else {

    ProtocolInterface *protocol = m_protocols.value(item.m_protocol_name);
	if(!protocol)
		return;

	protocol->chatWindowClosed(item.m_account_name, item.m_item_name);
    }
}

void PluginSystem::createConference(const QString &protocol_name,
		const QString &conference_name,
		const QString &account_name)
{
	TreeModelItem item;
	item.m_account_name = account_name;
	item.m_parent_name = account_name;
	item.m_protocol_name = protocol_name;
	item.m_item_name = conference_name;
	item.m_item_type = 32;
	if(AbstractLayer::Chat())
	AbstractLayer::Chat()->createChat(item);
   // AbstractChatLayer::instance().createConference(protocol_name,
			//conference_name, account_name);
}

void PluginSystem::addMessageToConference(const QString &protocol_name,
		const QString &conference_name, const QString &account_name,
		const QString &from,
		const QString &message, const QDateTime &date, bool history)
{
		TreeModelItem item;
	item.m_account_name = account_name;
	item.m_parent_name = conference_name;
	item.m_protocol_name = protocol_name;
	item.m_item_name = from;
	item.m_item_type = 34;

	if(AbstractLayer::Chat())
	AbstractLayer::Chat()->newMessageArrivedTo(item,message, date, history);


	//TODO: delete it later

    /*AbstractChatLayer::instance().addMessageToConference(
			protocol_name,
			conference_name,account_name,
			from, message, date, history);*/
}

void PluginSystem::sendMessageToConference(
		const QString &protocol_name,
		const QString &conference_name, const QString &account_name,
		const QString &message)
{
	ProtocolInterface *protocol = m_protocols.value(protocol_name);
	if(!protocol)
		return;

	protocol->sendMessageToConference(conference_name, account_name, message);
}

void PluginSystem::changeOwnConferenceNickName(const QString &protocol_name,
		const QString &conference_name,
		const QString &account_name,
		const QString &nickname)
{
	TreeModelItem item;
	item.m_account_name = account_name;
	item.m_parent_name = account_name;
	item.m_protocol_name = protocol_name;
	item.m_item_name = conference_name;
	item.m_item_type = 32;

	if(AbstractLayer::Chat())
	    AbstractLayer::Chat()->changeOwnNickNameInConference(item,nickname);

	/*AbstractChatLayer::instance().changeOwnConferenceNickName(protocol_name,
			conference_name, account_name, nickname);*/
}

void PluginSystem::setConferenceTopic(const QString &protocol_name, const QString &conference_name,
		const QString &account_name, const QString &topic)
{
	TreeModelItem item;
	item.m_account_name = account_name;
	item.m_parent_name = account_name;
	item.m_protocol_name = protocol_name;
	item.m_item_name = conference_name;
	item.m_item_type = 32;

	if(AbstractLayer::Chat())
	    AbstractLayer::Chat()->setConferenceTopic(item,topic);



    /*AbstractChatLayer::instance().setConferenceTopic(
			protocol_name, conference_name,
			account_name, topic);*/
}

void PluginSystem::addSystemMessageToConference(const QString &protocol_name,
		const QString &conference_name, const QString &account_name,
		const QString &message, const QDateTime &date, bool history)
{
    	TreeModelItem item;
	item.m_account_name = account_name;
	item.m_parent_name = account_name;
	item.m_protocol_name = protocol_name;
	item.m_item_name = conference_name;
	item.m_item_type = 32;

	if(AbstractLayer::Chat())
	    AbstractLayer::Chat()->newServiceMessageArriveTo(item,message);

	/*AbstractChatLayer::instance().addSystemMessageToConference(
			protocol_name, conference_name,account_name,
			message, date, history);*/
}

void PluginSystem::leaveConference(const QString &protocol_name,
		const QString &conference_name, const QString &account_name)
{
	ProtocolInterface *protocol = m_protocols.value(protocol_name);
	if(!protocol)
		return;

	protocol->leaveConference(conference_name, account_name);
}
void PluginSystem::addConferenceItem(const QString &protocol_name, const QString &conference_name,
		const QString &account_name, const QString &nickname)
{
	TreeModelItem item;
	item.m_account_name = account_name;
	item.m_parent_name = conference_name;
	item.m_protocol_name = protocol_name;
	item.m_item_name = nickname;
	item.m_item_type = 33;



    if(AbstractLayer::Chat())
	AbstractLayer::Chat()->addConferenceItem(item,nickname);
    //TODO: remove it later
    /*AbstractChatLayer &acl = AbstractChatLayer::instance();
	acl.addConferenceItem(protocol_name, conference_name, account_name, nickname);*/
	//END remove
}
void PluginSystem::removeConferenceItem(const QString &protocol_name, const QString &conference_name,
		const QString &account_name, const QString &nickname)
{
    	TreeModelItem item;
	item.m_account_name = account_name;
	item.m_parent_name = conference_name;
	item.m_protocol_name = protocol_name;
	item.m_item_name = nickname;
	item.m_item_type = 33;



    if(AbstractLayer::Chat())
	AbstractLayer::Chat()->removeConferenceItem(item);
    //TODO: remove it later
	/*AbstractChatLayer &acl = AbstractChatLayer::instance();
	acl.removeConferenceItem(protocol_name, conference_name, account_name, nickname);	*/
}
void PluginSystem::renameConferenceItem(const QString &protocol_name, const QString &conference_name, const QString &account_name,
		const QString &nickname, const QString &new_nickname)
{
		TreeModelItem item;
	item.m_account_name = account_name;
	item.m_parent_name = conference_name;
	item.m_protocol_name = protocol_name;
	item.m_item_name = nickname;
	item.m_item_type = 33;



    if(AbstractLayer::Chat())
	AbstractLayer::Chat()->renameConferenceItem(item, new_nickname);
    //TODO: remove it later
   /* AbstractChatLayer &acl = AbstractChatLayer::instance();
	acl.renameConferenceItem(protocol_name, conference_name, account_name, nickname, new_nickname);	*/
}
void PluginSystem::setConferenceItemStatus(const QString &protocol_name, const QString &conference_name, const QString &account_name,
		const QString &nickname, const QIcon &icon, const QString &status, int mass)
{
		TreeModelItem item;
	item.m_account_name = account_name;
	item.m_parent_name = conference_name;
	item.m_protocol_name = protocol_name;
	item.m_item_name = nickname;
	item.m_item_type = 33;



    if(AbstractLayer::Chat())
	AbstractLayer::Chat()->setConferenceItemStatus(item,icon,status,mass);
    //TODO: remove it later
    /*AbstractChatLayer &acl = AbstractChatLayer::instance();
	acl.setConferenceItemStatus(protocol_name, conference_name, account_name, nickname, icon, status, mass);*/
}
void PluginSystem::setConferenceItemIcon(const QString &protocol_name, const QString &conference_name, const QString &account_name, const QString &nickname,
		const QIcon &icon, int position)
{
		TreeModelItem item;
	item.m_account_name = account_name;
	item.m_parent_name = conference_name;
	item.m_protocol_name = protocol_name;
	item.m_item_name = nickname;
	item.m_item_type = 33;



    if(AbstractLayer::Chat())
	AbstractLayer::Chat()->setConferenceItemIcon(item,icon,position);
    //TODO: remove it later
    /*
    AbstractChatLayer &acl = AbstractChatLayer::instance();
	acl.setConferenceItemIcon(protocol_name, conference_name, account_name, nickname, icon, position);*/
}
void PluginSystem::setConferenceItemRole(const QString &protocol_name, const QString &conference_name, const QString &account_name,
		const QString &nickname, const QIcon &icon, const QString &role, int mass)
{
		TreeModelItem item;
	item.m_account_name = account_name;
	item.m_parent_name = conference_name;
	item.m_protocol_name = protocol_name;
	item.m_item_name = nickname;
	item.m_item_type = 33;



    if(AbstractLayer::Chat())
	AbstractLayer::Chat()->setConferenceItemRole(item,icon,role,mass);
    //TODO: remove it later
    /*AbstractChatLayer &acl = AbstractChatLayer::instance();
	acl.setConferenceItemRole(protocol_name, conference_name, account_name, nickname, icon, role, mass);*/
}
QStringList PluginSystem::getConferenceItemsList(const QString &protocol_name,
		const QString &conference_name, const QString &account_name)
{
	TreeModelItem item;
	item.m_account_name = account_name;
	item.m_parent_name = account_name;
	item.m_protocol_name = protocol_name;
	item.m_item_name = conference_name;
	item.m_item_type = 32;


	return AbstractLayer::Chat()->getConferenceItemsList(item);
}
void PluginSystem::setItemVisible(const TreeModelItem &item, bool visible)
{
	AbstractLayer::ContactList()->setItemVisibility(item, visible?ShowAlwaysVisible:ShowDefault);
//	AbstractContactList::instance().setItemVisible(item, visible);
}
void PluginSystem::setItemInvisible(const TreeModelItem &item, bool invisible)
{
	AbstractLayer::ContactList()->setItemVisibility(item, invisible?ShowAlwaysInvisible:ShowDefault);
//	AbstractContactList::instance().setItemInvisible(item, invisible);
}

void PluginSystem::conferenceItemActivated(const QString &protocol_name, const QString &conference_name,
		const QString &account_name, const QString &nickname)
{
	ProtocolInterface *protocol = m_protocols.value(protocol_name);
	if(!protocol)
		return;

	protocol->conferenceItemActivated(conference_name, account_name, nickname);
}

void PluginSystem::conferenceItemContextMenu(const QList<QAction*> &action_list, const QString &protocol_name, 
		const QString &conference_name, const QString &account_name, const QString &nickname, const QPoint &menu_point)
{
	ProtocolInterface *protocol = m_protocols.value(protocol_name);
	if(!protocol)
		return;

	protocol->conferenceItemContextMenu(action_list, conference_name, account_name, nickname, menu_point);	
}

QString PluginSystem::getConferenceItemToolTip(const QString &protocol_name, const QString &conference_name,
		const QString &account_name, const QString &nickname)
{
	ProtocolInterface *protocol = m_protocols.value(protocol_name);
	if(!protocol)
		return nickname;

	return protocol->getConferenceItemToolTip(conference_name, account_name, nickname);	
}

void PluginSystem::showConferenceContactInformation(const QString &protocol_name,
		const QString &conference_name, const QString &account_name, const QString &nickname)
{
	ProtocolInterface *protocol = m_protocols.value(protocol_name);
	if(!protocol)
		return;

	protocol->showConferenceContactInformation(conference_name, account_name, nickname);	
}

void PluginSystem::removePluginsSettingsWidget()
{
	for(int i = 0; i < m_plugins.count(); ++i)
	{
		SimplePluginInterface *plugin = getPluginByIndex(i);
		if(plugin) {
			plugin->removeSettingsWidget();
		}
	}
}

void PluginSystem::saveAllPluginsSettings()
{
	for(int i = 0; i < m_plugins.count(); ++i)
	{
		SimplePluginInterface *plugin = getPluginByIndex(i);
		if(plugin) {
			plugin->saveSettings();
		}
	}
}

void PluginSystem::registerContactMenuAction(QAction *plugin_action, DeprecatedSimplePluginInterface *pointer_this)
{
	AbstractContextLayer::instance().registerContactMenuAction(plugin_action);
	if(pointer_this)
		m_registered_events_plugins.insert(ContactContextAction, pointer_this);
}

void PluginSystem::setCurrentContextItemToPlugins(const TreeModelItem &item)
{
	TreeModelItem tmp_item = item;
	Event(m_events->contact_context, 1, &tmp_item).send();
	foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(ContactContextAction))
	{
		PluginEvent event;
		event.system_event_type = ContactContextAction;
		QList<void *> params;
		params.append(&tmp_item);
		event.args = params;
		plugin->processEvent(event);
	}
}

void PluginSystem::sendCustomMessage(const TreeModelItem &item, const QString &message, bool silent)
{
	if ( silent )
	{
		ProtocolInterface *protocol = m_protocols.value(item.m_protocol_name);
			if(!protocol)
				return;
			protocol->sendMessageTo(item.m_account_name, item.m_item_name, item.m_item_type, message, 0);
	}
	else
	    AbstractLayer::Chat()->newMessageArrivedTo(item,message, QDateTime::currentDateTime(), false,false);
}

void PluginSystem::registerMainMenuAction(QAction *menu_action)
{
	AbstractLayer::instance().addActionToMainMenu(menu_action);
}

void PluginSystem::showTopicConfig(const QString &protocol_name, const QString &account_name, const QString &conference)
{
	ProtocolInterface *protocol = m_protocols.value(protocol_name);
	if(!protocol)
		return;
	protocol->showConferenceTopicConfig(conference, account_name);
}

void PluginSystem::showConferenceMenu(const QString &protocol_name, const QString &account_name, const QString &conference_name,
		const QPoint &menu_point)
{
	ProtocolInterface *protocol = m_protocols.value(protocol_name);
	if(!protocol)
		return;
	protocol->showConferenceMenu(conference_name, account_name, menu_point);
}

void PluginSystem::redirectEventToProtocol(const QStringList &protocol_name, const QList<void*> &event)
{
	if ( !protocol_name.count() )
	{
		foreach(ProtocolInterface *protocol, m_protocols)
		{
			protocol->getMessageFromPlugins(event);
		}
	} else
	{
		foreach(QString protocol_n, protocol_name)
		{
			ProtocolInterface *protocol = m_protocols.value(protocol_n);
			if(protocol)
				protocol->getMessageFromPlugins(event);
		}
	}

}

void PluginSystem::sendMessageBeforeShowing(const TreeModelItem &item, QString &message)
{
	TreeModelItem tmp_item = item;
	Event event(m_events->sending_message_before_showing, 2, &tmp_item, &message);
	sendEvent(event);
	foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(SendingMessageBeforeShowing))
	{
		PluginEvent event;
		event.system_event_type = SendingMessageBeforeShowing;
		QList<void *> params;
		params.append(&tmp_item);
		params.append(&message);
		event.args = params;
		plugin->processEvent(event);
	}
}

void PluginSystem::pointersAreInitialized()
{
	Event event(m_events->pointers_are_initialized, 0);
	sendEvent(event);
	foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(PointersAreInitialized))
	{
		PluginEvent event;
		event.system_event_type = PointersAreInitialized;
		plugin->processEvent(event);
	}
}

void PluginSystem::playSoundByPlugin(QString path)
{	
	foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(SoundAction))
	{
		PluginEvent event;
		event.system_event_type = SoundAction;
		QList<void *> params;
		params.append(&path);
		plugin->processEvent(event);
	}
}

void PluginSystem::playSound(NotificationType event)
{
//	AbstractLayer::Notification()->playSound(event);
}

void PluginSystem::playSound(const QString &file_name)
{
	AbstractLayer::SoundEngine()->playSound(file_name);
}

void PluginSystem::receivingMessageBeforeShowing(const TreeModelItem &item, QString &message)
{
	TreeModelItem tmp_item = item;
	Event event(m_events->receiving_message_third_level, 2, &tmp_item, &message);
	sendEvent(event);
	foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(ReceivingMessageThirdLevel))
	{
		PluginEvent event;
		event.system_event_type = ReceivingMessageThirdLevel;
		QList<void *> params;
		params.append(&tmp_item);
		params.append(&message);
		event.args = params;
		plugin->processEvent(event);
	}
	event.id = m_events->receiving_message_fourth_level;
	sendEvent(event);
	foreach(DeprecatedSimplePluginInterface *plugin, m_registered_events_plugins.values(ReceivingMessageFourthLevel))
	{
		PluginEvent event;
		event.system_event_type = ReceivingMessageFourthLevel;
		QList<void *> params;
		params.append(&tmp_item);
		params.append(&message);
		event.args = params;
		plugin->processEvent(event);
	}
}

void PluginSystem::editAccount(const QString &protocol_name, const QString &account_name)
{
	ProtocolInterface *protocol = m_protocols.value(protocol_name);
	if(!protocol)
		return;

	protocol->editAccount(account_name);
}

bool PluginSystem::changeChatWindowID(const TreeModelItem &item, const QString &id)
{
	//return AbstractChatLayer::instance().changeChatWindowID(item, id);

	Event event(registerEventHandler("Core/ChatWindow/ChangeID"), 2, &item, &id);
	sendEvent(event);
	return true;
}

quint16 PluginSystem::registerEventHandler(const QString &event_id, EventHandler *handler, quint16 priority)
{
	quint16 id = m_events_id.value(event_id, m_new_event_id);
	if(id == m_new_event_id)
	{
		m_events_id.insert(event_id, m_new_event_id);
		m_new_event_id++;
		m_event_handlers.resize(m_new_event_id);
	}
	if(handler)
	{
		EventHandlerInfo info;
		info.first = priority;
		info.second = new Track( handler );
		int position = qUpperBound(m_event_handlers[id], info) - m_event_handlers[id].constBegin();
		m_event_handlers[id].insert(position, info);
	}
	return id;
}

void PluginSystem::removeEventHandler(quint16 id, EventHandler *handler)
{
	if(id >= m_event_handlers.size() || !handler)
		return;
	QVector<EventHandlerInfo> &handlers = m_event_handlers[id];
	for( int i=handlers.size()-1; i>=0; i-- )
		if( handlers[i].second->handler == handler )
		{
			delete handlers[i].second;
			handlers.remove(i);
		}
}

void PluginSystem::removeEventHandler( EventHandler *handler )
{
	if( !handler )
		return;
	for( int i=0; i<m_event_handlers.size(); i++ )
	{
		QVector<EventHandlerInfo> &handlers = m_event_handlers[i];
		for( int j=handlers.size()-1; j>=0; j-- )
		{
			if( handlers[j].second->handler == handler )
			{
				delete handlers[j].second;
				handlers.remove(j);
			}
		}
	}
}

quint16 PluginSystem::registerEventHandler(const QString &event_id, EventHandler *handler, EventHandlerFunc member, quint16 priority)
{
	quint16 id = m_events_id.value(event_id, m_new_event_id);
	if(id == m_new_event_id)
	{
		m_events_id.insert(event_id, m_new_event_id);
		m_new_event_id++;
		m_event_handlers.resize(m_new_event_id);
	}
	if( handler && member )
	{
		EventHandlerInfo info;
		info.first = priority;
		info.second = new Track( handler, member );
		int position = qUpperBound(m_event_handlers[id], info) - m_event_handlers[id].constBegin();
		m_event_handlers[id].insert(position, info);
	}
	return id;
}

void PluginSystem::removeEventHandler(quint16 id, EventHandler *handler, EventHandlerFunc member)
{
	if(id >= m_event_handlers.size() || !handler || !member)
		return;
	QVector<EventHandlerInfo> &handlers = m_event_handlers[id];
	for( int i=handlers.size()-1; i>=0; i-- )
		if( handlers[i].second->handler == handler && handlers[i].second->method == member )
		{
			delete handlers[i].second;
			handlers.remove(i);
		}
}

void PluginSystem::removeEventHandler( EventHandler *handler, EventHandlerFunc member )
{
	if( !handler || !member )
		return;
	for( int i=0; i<m_event_handlers.size(); i++ )
	{
		QVector<EventHandlerInfo> &handlers = m_event_handlers[i];
		for( int j=handlers.size()-1; j>=0; j-- )
		{
			if( handlers[j].second->handler == handler && handlers[j].second->method == member )
			{
				delete handlers[j].second;
				handlers.remove(j);
			}
		}
	}
}

quint16 PluginSystem::registerEventHandler(const QString &event_id, ProcessEventFunc func, quint16 priority)
{
	quint16 id = m_events_id.value(event_id, m_new_event_id);
	if(id == m_new_event_id)
	{
		m_events_id.insert(event_id, m_new_event_id);
		m_new_event_id++;
		m_event_handlers.resize(m_new_event_id);
	}
	if( func )
	{
		EventHandlerInfo info;
		info.first = priority;
		info.second = new Track( func );
		int position = qUpperBound(m_event_handlers[id], info) - m_event_handlers[id].constBegin();
		m_event_handlers[id].insert(position, info);
	}
	return id;
}

void PluginSystem::removeEventHandler(quint16 id, ProcessEventFunc func)
{
	if(id >= m_event_handlers.size() || !func )
		return;
	QVector<EventHandlerInfo> &handlers = m_event_handlers[id];
	for( int i=handlers.size()-1; i>=0; i-- )
		if( handlers[i].second->func == func )
		{
			delete handlers[i].second;
			handlers.remove(i);
		}
}

void PluginSystem::removeEventHandler( ProcessEventFunc func )
{
	if( !func )
		return;
	for( int i=0; i<m_event_handlers.size(); i++ )
	{
		QVector<EventHandlerInfo> &handlers = m_event_handlers[i];
		for( int j=handlers.size()-1; j>=0; j-- )
		{
			if( handlers[j].second->func == func )
			{
				delete handlers[j].second;
				handlers.remove(j);
			}
		}
	}
}

quint16 PluginSystem::registerEventHandler(const QString &event_id, QObject *object, const char *member, quint16 priority )
{
	quint16 id = m_events_id.value(event_id, m_new_event_id);
	if(id == m_new_event_id)
	{
		m_events_id.insert(event_id, m_new_event_id);
		m_new_event_id++;
		m_event_handlers.resize(m_new_event_id);
	}
	if( object && member && ( (*member - '0') == QSIGNAL_CODE || (*member - '0') == QSLOT_CODE ) )
	{
		EventHandlerInfo info;
		info.first = priority;
		info.second = new Track( object, member );
		int position = qUpperBound(m_event_handlers[id], info) - m_event_handlers[id].constBegin();
		m_event_handlers[id].insert(position, info);
	}
	return id;
}

void PluginSystem::removeEventHandler(quint16 id, QObject *object, const char *member)
{
	if( id >= m_event_handlers.size() || !object )
		return;
	QVector<EventHandlerInfo> &handlers = m_event_handlers[id];
	int slot_id = member ? object->metaObject()->indexOfSlot(member) : -1;
	for( int i=handlers.size()-1; i>=0; i-- )
		if( handlers[i].second->object == object && ( slot_id < 0 || handlers[i].second->slot_id == slot_id ) )
		{
			delete handlers[i].second;
			handlers.remove(i);
		}
}

void PluginSystem::removeEventHandler( QObject *object )
{
	if( !object )
		return;
	for( int i=0; i<m_event_handlers.size(); i++ )
	{
		QVector<EventHandlerInfo> &handlers = m_event_handlers[i];
		for( int j=handlers.size()-1; j>=0; j-- )
		{
			if( handlers[j].second->object == object )
			{
				delete handlers[j].second;
				handlers.remove(j);
			}
		}
	}
}

bool PluginSystem::sendEvent(Event &event)
{
	if( event.id >= m_event_handlers.size() )
		return false;
	QVector<EventHandlerInfo> &handlers = m_event_handlers[event.id];
	if( !handlers.size() )
		return false;
	void **args = 0;
	for( int i=0; i<handlers.size(); i++ )
		switch( handlers[i].second->type )
		{
		case Track::Interface:
			handlers[i].second->handler->processEvent( event );
			break;
		case Track::Method:
			( handlers[i].second->handler->*handlers[i].second->method )( event );
			break;
		case Track::Func:
			( *handlers[i].second->func )( event );
			break;
		case Track::Slot:
		case Track::Signal:
			if( handlers[i].second->object.isNull() )
				continue;
			if( !args )
			{
				args = (void **)qMalloc( sizeof(void *) * ( event.size() + 1 ) );
				args[0] = 0;
				qMemCopy( args + 1, event.args.constData(), sizeof(void *) * event.args.size() );
			}
			if( handlers[i].second->type == Track::Slot )
				handlers[i].second->object->qt_metacall( QMetaObject::InvokeMetaMethod, handlers[i].second->slot_id, args );
			else
				QMetaObject::activate( handlers[i].second->object, handlers[i].second->slot_id, args );
			break;
		}
	if( args )
		qFree( args );
	return true;
}

void PluginSystem::getSystemInfo(QString &version, QString &timezone, int &timezone_offset)
{
	const qutim_sdk_0_3::SystemInfo &info = qutim_sdk_0_3::SystemInfo::instance();
	version = info.osFull();
	timezone = info.timezone();
	timezone_offset = info.timezoneOffset();
}

IconManagerInterface *PluginSystem::getIconManager()
{
	return &IconManager::instance();
}

TranslatorInterface *PluginSystem::getTranslator()
{
	return 0;
}

TranslatorInterface *PluginSystem::getTranslator( const QString &lang )
{
	return 0;
}

QStringList PluginSystem::availableTranslations()
{
	return QStringList();
}

SettingsInterface *PluginSystem::getSettings( const TreeModelItem &item )
{
	return m_settings ? m_settings->getSettings( item ) : 0;
}

SettingsInterface *PluginSystem::getSettings( const QString &name )
{
	return m_settings ? m_settings->getSettings( name ) : 0;
}

QString PluginSystem::getProfilePath()
{
	return m_settings ? m_settings->getProfilePath() : QString();
}

QDir PluginSystem::getProfileDir()
{
	return m_settings ? m_settings->getProfileDir() : QDir();
}

void PluginSystem::centerizeWidget(QWidget *widget)
{
	QRect rect = QApplication::desktop()->screenGeometry(QCursor::pos());
	QPoint position(rect.left() + rect.width() / 2 - widget->size().width() / 2,
	rect.top() + rect.height() / 2 - widget->size().height() / 2);
	widget->move(position);
}

QList<TreeModelItem> PluginSystem::getItemChildren(const TreeModelItem &item)
{
	return AbstractLayer::ContactList()->getItemChildren(item);
}

void PluginSystem::setItemVisibility(const TreeModelItem &item, int flags)
{
	return AbstractLayer::ContactList()->setItemVisibility(item, flags);
}

void PluginSystem::showCL()
{
	Q_REGISTER_EVENT( show_hide_cl, "Core/ContactList/ShowHide" );
	bool show = true;
	Event( show_hide_cl, 1, &show ).send();
}

void PluginSystem::setTrayMessageIconAnimating(bool animate)
{
    if (animate)
	m_abstract_layer.animateTrayNewMessage();
    else
	m_abstract_layer.stopTrayNewMessageAnimation();
}

/*
   ChatLayerClass

    Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "chatlayerclass.h"
#include "logscity.h"

ChatLayerClass::ChatLayerClass()
{
    m_settings_widget = 0;
    m_settings_item = 0;
    m_separate_management = 0;
    m_tabbed_management = 0;
}

ChatLayerClass::~ChatLayerClass()
{

}

void ChatLayerClass::release()
{
    saveUnreadedMessage();
	LogsCity::instance().release();
}

bool ChatLayerClass::init(PluginSystemInterface *plugin_system)
{
    m_plugin_system = plugin_system;
    TempGlobalInstance::instance().setPluginSystem(plugin_system);
	m_event_head = m_plugin_system->registerEventHandler("Core/ChatWindow/AppendHtmlToHead",this);
    return true;
}

QList<SettingsStructure> ChatLayerClass::getLayerSettingsList()
{
    m_settings.clear();
    if ( !m_settings_widget )
    {
	m_settings_widget = new ChatLayerSettings(m_profile_name);
	m_settings_item = new QTreeWidgetItem;
	m_settings_item->setText(0,tr("Chat window"));
	m_settings_item->setIcon(0,Icon("messaging"));
	SettingsStructure tmp_struct;
	tmp_struct.settings_item = m_settings_item;
	tmp_struct.settings_widget = m_settings_widget;
	m_settings.append(tmp_struct);
    }

    return m_settings;

}

void ChatLayerClass::saveLayerSettings()
{
    if ( m_settings_widget )
	m_settings_widget->saveSettings();
    loadSettings();
}


void ChatLayerClass::removeLayerSettings()
{
    if ( m_settings_widget )
    {
	delete m_settings_widget;
	m_settings_widget = 0;
	delete m_settings_item;
	m_settings_item = 0;
    }
}

void ChatLayerClass::saveGuiSettingsPressed()
{
	LogsCity::instance().loadGuiSettings();
}

void ChatLayerClass::setProfileName(const QString &profile_name)
{
    m_event_tray_clicked = m_plugin_system->registerEventHandler("Core/Tray/Clicked", this);
	m_event_show_hide_cl = m_plugin_system->registerEventHandler("Core/ContactList/ShowHide");
	m_event_change_id = m_plugin_system->registerEventHandler("Core/ChatWindow/ChangeID",this);
	m_event_js = m_plugin_system->registerEventHandler("Core/ChatWindow/EvaluateJavaScript",this);
	m_event_all_plugin_loaded = m_plugin_system->registerEventHandler("Core/AllPluginsLoaded",this);

    m_profile_name = profile_name;
    m_separate_management = new SeparateChats(profile_name);
    m_tabbed_management = new TabbedChats(profile_name);
    connect(m_tabbed_management, SIGNAL(restorePreviousTabs()), this, SLOT(restorePreviousChats()));
    loadSettings();

}

void ChatLayerClass::loadSettings()
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
    LogsCity::instance().m_tmp_image_path = settings.fileName().section('/', 0, -2) + "/messageimages/";
    settings.beginGroup("chatwindow");
    m_tabbed_mode = settings.value("tabbed", true).toBool();
    m_open_chat_on_new_message = settings.value("open", false).toBool();
    m_dont_show_events_if_message_chat_is_open = settings.value("events", false).toBool();
    m_show_all = settings.value("openall", true).toBool();

    settings.endGroup();
    m_play_sound_if_active = settings.value("sounds/actwndincome",false).toBool();

    if ( m_separate_management ) m_separate_management->loadSettings();
    if ( m_tabbed_management ) m_tabbed_management->loadSettings();

    LogsCity::instance().loadSettings(m_profile_name);
}

void ChatLayerClass::createChat(const TreeModelItem &item)
{
    createChat(item,false);
}

void ChatLayerClass::createChat(const TreeModelItem &item,bool new_message)
{
    TempGlobalInstance &tmp_tgi = TempGlobalInstance::instance();

    if ( !LogsCity::instance().doIHaveHome(item) )
    {
		tmp_tgi.chatAboutToBeOpened(item);
		QStringList item_info = tmp_tgi.getItemInfo(item);
		TreeModelItem owner_item ;
		owner_item.m_account_name = item.m_account_name;
		owner_item.m_item_name = item.m_account_name;
		owner_item.m_item_type = item.m_item_type;
		owner_item.m_protocol_name = item.m_protocol_name;
		owner_item.m_parent_name = item.m_account_name;

		QStringList owner_info = tmp_tgi.getItemInfo(owner_item);

		if (m_tabbed_mode && m_tabbed_management )
			m_tabbed_management->createChat(item,item_info, owner_info,new_message);
		else if (!m_tabbed_mode && m_separate_management)
			m_separate_management->createChat(item,item_info, owner_info);
		loadHistoryMessages(item);
		checkForNewMessages(item);
		tmp_tgi.chatOpened(item);
    } else {
		if (m_tabbed_mode && m_tabbed_management )
			m_tabbed_management->activateWindow(item,new_message);
		else if (!m_tabbed_mode && m_separate_management)
			m_separate_management->activateWindow(item);
    }
}

void ChatLayerClass::loadGuiSettings()
{
}

void ChatLayerClass::setLayerInterface( LayerType type, LayerInterface *linterface)
{
    TempGlobalInstance::instance().setLayerInterface(type,linterface);

    switch(type)
    {
	case NotificationLayer:
	    m_notification_layer = reinterpret_cast<NotificationLayerInterface *>(linterface);
	    break;
	case HistoryLayer:
	    m_history_layer = reinterpret_cast<HistoryLayerInterface *>(linterface);
	    break;
	default:;
    }
}

void ChatLayerClass::contactChangeHisStatus(const TreeModelItem &item, const QIcon &icon)
{
	if ( LogsCity::instance().doIHaveHome(item) )
	{
		if (m_tabbed_mode && m_tabbed_management )
			m_tabbed_management->contactChangeHisStatus(item,icon);
		else if (!m_tabbed_mode && m_separate_management)
			m_separate_management->contactChangeHisStatus(item,icon);
	}
}

void ChatLayerClass::contactChangeHisClient(const TreeModelItem &item)
{
	if ( LogsCity::instance().doIHaveHome(item) )
	{
		if (m_tabbed_mode && m_tabbed_management )
			m_tabbed_management->contactChangeHisClient(item);
		else if (!m_tabbed_mode && m_separate_management)
			m_separate_management->contactChangeHisClient(item);
	}
}

void ChatLayerClass::addConferenceItem(const TreeModelItem &item, const QString &name)
{
    LogsCity::instance().addConferenceItem(item,name);
}

void ChatLayerClass::setConferenceItemStatus(const TreeModelItem &item, const QIcon &icon, const QString &status, int mass)
{
    LogsCity::instance().setConferenceItemStatus(item,icon,status,mass);
}

void ChatLayerClass::renameConferenceItem(const TreeModelItem &item, const QString &new_name)
{
    LogsCity::instance().renameConferenceItem(item,new_name);
}

void ChatLayerClass::removeConferenceItem(const TreeModelItem &item)
{
    LogsCity::instance().removeConferenceItem(item);
}

void ChatLayerClass::setConferenceItemIcon(const TreeModelItem &item, const QIcon &icon, int position)
{
    LogsCity::instance().setConferenceItemIcon(item,icon,position);
}

void ChatLayerClass::setConferenceItemRole(const TreeModelItem &item, const QIcon &icon, const QString &role, int mass)
{
    LogsCity::instance().setConferenceItemRole(item,icon,role,mass);
}

void ChatLayerClass::newMessageArrivedTo(const TreeModelItem &item, const QString &message,
				     const QDateTime &date, bool history, bool in)
{
	bool notify_about_message = false;
	bool window_open = false;
	if ( item.m_item_type == 34 )
	{
	    bool win_active = true;
	    if (m_tabbed_mode && m_tabbed_management )
		 win_active = !m_tabbed_management->checkForActivation(item,false);
	    else if (!m_tabbed_mode && m_separate_management)
		 win_active = !m_separate_management->checkForActivation(item,false);
	    if (m_tabbed_mode) m_tabbed_management->updateWebkit(false);
	    if ( LogsCity::instance().addMessage(item,message,date,history,in, win_active) )
	    {
		if (m_tabbed_mode && m_tabbed_management )
		    m_tabbed_management->alertWindow(item);
		else if (!m_tabbed_mode && m_separate_management)
		    m_separate_management->alertWindow(item);
	    }
	    if (m_tabbed_mode) m_tabbed_management->updateWebkit(true);
	    return;
	}
	if ( m_open_chat_on_new_message ) createChat(item,true);
	if ( LogsCity::instance().doIHaveHome(item) )
	{
	    window_open = true;

		bool add_to_waiting_activation_list = false;
		if (m_tabbed_mode && m_tabbed_management )
		    add_to_waiting_activation_list = m_tabbed_management->checkForActivation(item);
		else if (!m_tabbed_mode && m_separate_management)
		    add_to_waiting_activation_list =  m_separate_management->checkForActivation(item);

		if(add_to_waiting_activation_list && item.m_item_type != 32)
		{
		    notify_about_message = true;
		    TempGlobalInstance &temp_global = TempGlobalInstance::instance();
		    temp_global.setTrayMessageIconAnimating(true);
		    temp_global.notifyAboutUnreadedMessage(item);
		    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
			.arg(item.m_account_name).arg(item.m_item_name);
		    if ( !temp_global.m_waiting_for_activation.contains(identification) )
			temp_global.m_waiting_for_activation.insert(identification,item);
		}
		if (m_tabbed_mode) m_tabbed_management->updateWebkit(false);
		LogsCity::instance().addMessage(item,message,date,history,in);
		if (m_tabbed_mode) m_tabbed_management->updateWebkit(true);
	}
	else
	{
	   notify_about_message = true;
	   TempGlobalInstance &temp_global = TempGlobalInstance::instance();
	   temp_global.setTrayMessageIconAnimating(true);
	   temp_global.notifyAboutUnreadedMessage(item);
	   QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
		.arg(item.m_account_name).arg(item.m_item_name);
	   UnreadedMessage tmp_message;
	   tmp_message.m_item = item;
	   tmp_message.m_date = date;
	   tmp_message.m_message = message;

	   if ( !temp_global.m_unreaded_messages_list.contains(identification) )
	       temp_global.m_unreaded_messages_list.insert(identification, new QVector<UnreadedMessage>);

	   temp_global.m_unreaded_messages_list.value(identification)->append(tmp_message);

	}
	if ( window_open && m_dont_show_events_if_message_chat_is_open )
	    return;

	const ItemData *data = LayersCity::ContactList()->getItemData(item);
	if(data && !(data->visibility & ShowMessage))
		return;

	if ( notify_about_message )
	    m_notification_layer->userMessage(item, message, NotifyMessageGet);
	else if (window_open && m_play_sound_if_active)
	    m_notification_layer->playSound(item,NotifyMessageGet);
}

void ChatLayerClass::processEvent(Event &e)
{
	if(e.id == m_event_tray_clicked)
	{
	    TempGlobalInstance &temp_global = TempGlobalInstance::instance();
	    if (temp_global.m_unreaded_messages_list.count() || temp_global.m_waiting_for_activation.count())
	    {
		readAllUnreaded();
	    }else{
		Event ev(m_event_show_hide_cl);
		m_plugin_system->sendEvent(ev);
	    }
	} else if (e.id == m_event_change_id && e.size() >= 2 )
	{
		TreeModelItem &item = e.at<TreeModelItem>(0);
		QString &new_id = e.at<QString>(1);
		if ( LogsCity::instance().doIHaveHome(item) )
		{
			if (m_tabbed_mode && m_tabbed_management )
				m_tabbed_management->changeId(item,new_id);
			else if (!m_tabbed_mode && m_separate_management)
				m_separate_management->changeId(item,new_id);
		}
	} else if(e.id == m_event_head )
	{
		LogsCity::instance().appendHtmlToHead( e.at<QString>(0), e.at<QList<quint8> >(1) );
	} else if(e.id == m_event_js )
	{
		QVariant result;
		if( QWebPage *page = LogsCity::instance().giveMeMyHomeWebPage( e.at<TreeModelItem>(0) ) )
			result = page->mainFrame()->evaluateJavaScript( e.at<QString>(1) );
		if( e.size() == 3 )
			e.at<QVariant>(2) = result;
	} else if(e.id == m_event_all_plugin_loaded)
	{
		restoreUnreadedMessages();
	}
}

void ChatLayerClass::checkForNewMessages(const TreeModelItem &item)
{
   QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
   TempGlobalInstance &temp_global = TempGlobalInstance::instance();
   if ( temp_global.m_unreaded_messages_list.contains(identification))
   {
	QVector<UnreadedMessage> *temp_list = temp_global.m_unreaded_messages_list.value(identification);
	TreeModelItem tmp_item;
	for(int i=0; i<temp_list->size(); i++)
	{
	    UnreadedMessage tmp_message = temp_list->at(i);
	    LogsCity::instance().addMessage(tmp_message.m_item, tmp_message.m_message, tmp_message.m_date,false,true);
	    tmp_item = tmp_message.m_item;
	}

	delete temp_list;
	temp_global.m_unreaded_messages_list.remove(identification);
	temp_global.notifyAboutReadedMessage(tmp_item);
   }
   if ( temp_global.m_waiting_for_activation.contains(identification) )
   {
	temp_global.m_waiting_for_activation.remove(identification);
	temp_global.notifyAboutReadedMessage(item);
   }
}

void ChatLayerClass::readAllUnreaded()
{
    TempGlobalInstance &temp_global = TempGlobalInstance::instance();
    if (m_show_all)
    {
		foreach(QVector<UnreadedMessage> *temp_list, temp_global.m_unreaded_messages_list.values())
		{
			TreeModelItem tmp_item = temp_list->at(0).m_item;
			createChat(tmp_item);
		}
		foreach(TreeModelItem tmp_item, temp_global.m_waiting_for_activation.values())
		{
			createChat(tmp_item);
		}
		} else {
		if ( temp_global.m_unreaded_messages_list.count() )
		{
			TreeModelItem tmp_item =
				temp_global.m_unreaded_messages_list.begin().value()->at(0).m_item;
			createChat(tmp_item);
		} else if (temp_global.m_waiting_for_activation.count() )
			createChat(temp_global.m_waiting_for_activation.begin().value());
    }
}

void ChatLayerClass::setItemTypingState(const TreeModelItem &item, TypingAttribute state)
{

	bool item_has_home = LogsCity::instance().doIHaveHome(item) ;
	if (item_has_home)
	{
	    if (m_tabbed_mode && m_tabbed_management )
		m_tabbed_management->setItemTypingState(item,state);
	    else if (!m_tabbed_mode && m_separate_management)
		m_separate_management->setItemTypingState(item,state);
	}
	if ( (bool)state )
	{
	    bool notify_about_typing = false;;
	    if ( item_has_home )
	    {
		if (m_tabbed_mode && m_tabbed_management )
		    notify_about_typing = !m_tabbed_management->checkForActivation(item,true);
		else if (!m_tabbed_mode && m_separate_management)
		    notify_about_typing =  !m_separate_management->checkForActivation(item,true);

	    } else notify_about_typing = true;
	    if ( item_has_home && !m_dont_show_events_if_message_chat_is_open )
		return;
	    if ( notify_about_typing )
		m_notification_layer->userMessage(item, "", NotifyTyping);
	}

}

void ChatLayerClass::saveUnreadedMessage()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	QString unread_file = settings.fileName().section('/', 0, -2) + "/nunreaded.list";
	QFile u_file(unread_file);

	if ( u_file.open(QIODevice::WriteOnly) )
	{
	    QDataStream out(&u_file);
	    foreach(QVector<UnreadedMessage> *temp_list, TempGlobalInstance::instance().m_unreaded_messages_list)
		{


		    for(int i=0; i<temp_list->size(); i++)
		    {
			UnreadedMessage tmp_message = temp_list->at(i);
			
			out<<tmp_message.m_item.m_protocol_name
				<<tmp_message.m_item.m_account_name
				<<tmp_message.m_item.m_parent_name
				<<tmp_message.m_item.m_item_name
				<<tmp_message.m_message
				<<tmp_message.m_date;
		    }


		}
		u_file.close();
	}
}

void ChatLayerClass::restoreUnreadedMessages()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	QString unread_file = settings.fileName().section('/', 0, -2) + "/nunreaded.list";
	QFile u_file(unread_file);

	if ( u_file.open(QIODevice::ReadOnly) )
	{
		QDataStream in(&u_file);
		while( !in.atEnd() )
		{
			QString p,a,g,i,m;
			QDateTime time;
			in>>p>>a>>g>>i>>m>>time;
			TreeModelItem contact_item;
			contact_item.m_protocol_name = p;
			contact_item.m_account_name = a;
			contact_item.m_parent_name = g;
			contact_item.m_item_name = i;
			contact_item.m_item_type = 0;
			newMessageArrivedTo(contact_item,m,time);

		}
		u_file.close();
		u_file.remove();
	}
}

void ChatLayerClass::restorePreviousChats()
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	QString windows_file = settings.fileName().section('/', 0, -2) + "/owindows2.list";
	QFile w_file(windows_file);
	if ( w_file.open(QIODevice::ReadOnly) )
	{
		QDataStream in(&w_file);
		while( !in.atEnd() )
		{
			QString p,a,g,i;
			quint8 type;
			in>>p>>a>>g>>i>>type;
			TreeModelItem contact_item;
			contact_item.m_protocol_name = p;
			contact_item.m_account_name = a;
			contact_item.m_item_name = i;
			contact_item.m_parent_name = g;
			contact_item.m_item_type = type;

			createChat(contact_item);
		}
		w_file.close();
		w_file.remove();
	}
}

void ChatLayerClass::messageDelievered(const TreeModelItem &item, int message_position)
{
    if (m_tabbed_mode) m_tabbed_management->updateWebkit(false);
    LogsCity::instance().messageDelievered(item, message_position);
    if (m_tabbed_mode) m_tabbed_management->updateWebkit(true);
}

void ChatLayerClass::changeOwnNickNameInConference(const TreeModelItem &item, const QString &new_nickname)
{
    LogsCity::instance().changeOwnNickNameInConference(item, new_nickname);
}

void ChatLayerClass::newServiceMessageArriveTo(const TreeModelItem &item, const QString &message)
{
    if (m_tabbed_mode) m_tabbed_management->updateWebkit(false);
    LogsCity::instance().addServiceMessage(item,message);
    if (m_tabbed_mode) m_tabbed_management->updateWebkit(true);
}

void ChatLayerClass::setConferenceTopic(const TreeModelItem &item, const QString &topic)
{
    if (m_tabbed_mode && m_tabbed_management )
	m_tabbed_management->setConferenceTopic(item,topic);
    else if (!m_tabbed_mode && m_separate_management)
	m_separate_management->setConferenceTopic(item,topic);
}

QStringList ChatLayerClass::getConferenceItemsList(const TreeModelItem &item)
{
	return QStringList();
}

void ChatLayerClass::addImage(const TreeModelItem &item, const QByteArray &image_raw)
{
    LogsCity::instance().addImage(item,image_raw,true);
}

void ChatLayerClass::loadHistoryMessages(const TreeModelItem &item)
{
       QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
    QDateTime last_time = QDateTime::currentDateTime();
    QVector<UnreadedMessage> *temp_list = TempGlobalInstance::instance().m_unreaded_messages_list.value(identification);
    if ( temp_list)
    {
    for(int i=0; i<temp_list->size(); i++)
    {
	UnreadedMessage tmp_message = temp_list->at(i);
	if(tmp_message.m_date<last_time)
		last_time = tmp_message.m_date;
    }
    }
    QList<HistoryItem> tmp_h_list = m_history_layer->getMessages(item,last_time);

    foreach(HistoryItem h_item, tmp_h_list)
    {
	LogsCity::instance().addMessage(item,h_item.m_message, h_item.m_time,true, h_item.m_in);
    }
}

QTextEdit *ChatLayerClass::getEditField(const TreeModelItem &item)
{
	if(m_tabbed_mode)
		return m_tabbed_management->getEditField(item);
	else
		return m_separate_management->getEditField(item);
}

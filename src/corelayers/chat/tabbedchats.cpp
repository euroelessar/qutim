/*
  TabbedChats

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

#include "tabbedchats.h"
#include <QSettings>
#include <QMouseEvent>
TabbedChats::TabbedChats(const QString &profile_name) : m_profile_name(profile_name),
m_logs_city(LogsCity::instance())
{
    m_merge_conf_and_chats = false;
    m_merged_window.m_tab_bar = 0;
    m_merged_window.m_main_window = 0;
    m_merged_window.m_main_layout = 0;

    m_private_chats_window.m_tab_bar = 0;
    m_private_chats_window.m_main_window = 0;
    m_private_chats_window.m_main_layout = 0;

    m_all_conference_window.m_tab_bar = 0;
    m_all_conference_window.m_main_window = 0;
    m_all_conference_window.m_main_layout = 0;

    m_chat_window = 0;
    m_conference_window = 0;

}

TabbedChats::~TabbedChats()
{
    delete m_chat_window;
    delete m_conference_window;

    delete m_merged_window.m_tab_bar;
    delete m_merged_window.m_main_layout;
    delete m_merged_window.m_main_window;

    delete m_private_chats_window.m_tab_bar;
    delete m_private_chats_window.m_main_layout;
    delete m_private_chats_window.m_main_window;

    delete m_all_conference_window.m_tab_bar;
    delete m_all_conference_window.m_main_layout;
    delete m_all_conference_window.m_main_window;

}

void TabbedChats::loadSettings()
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
    settings.beginGroup("chatwindow");
    m_merge_conf_and_chats = settings.value("merge", false).toBool();
    m_webkit_mode = settings.value("webkit", true).toBool();
    m_closable_tabs = settings.value("tabsclosable", true).toBool();
    m_movable_tabs = settings.value("tabsmovable", true).toBool();
    m_remember_privates = settings.value("remember", false).toBool();
	m_close_after_send = settings.value("close", false).toBool();
	m_send_on_double_enter = settings.value("ondoubleenter", false).toBool();
	m_send_on_enter = m_send_on_double_enter || settings.value("onenter", false).toBool();
    m_send_typing_notifications = settings.value("typing", true).toBool();
    m_dont_blink = settings.value("dontblink", false).toBool();
    settings.endGroup();
}

void TabbedChats::createChat(const TreeModelItem &item, const QStringList &item_info,const QStringList &owner_info,bool new_message)
{
    TabbedWindow m_current_window;
    m_logs_city.createHomeForMe(item,m_webkit_mode, item_info.count() > 0 ? item_info.at(0) : item.m_item_name,
				owner_info.count() > 0 ? owner_info.at(0) : item.m_account_name,
				owner_info.count() > 1 ? owner_info.at(1) : "",
				item_info.count() > 1 ? item_info.at(1) : "");
    bool new_tabbed_window_was_created = false;
    if ( m_merge_conf_and_chats || m_merged_window.m_main_window)
    {
	if ( !m_merged_window.m_main_window )
	{
	    createTabbedWindow(&m_merged_window);
	    connect(m_merged_window.m_main_window, SIGNAL(destroyed(QObject*)), this, SLOT(destroyMergedWindow(QObject*)));
	    restoreSizeAndPosition(m_merged_window.m_main_window,MergedWindow);
	    new_tabbed_window_was_created = true;
	    m_merged_window.m_main_window->show();
	}


	if ( item.m_item_type != 32)
	{
	    if ( !m_chat_window )
	    {
		m_chat_window = new ChatWindow(m_profile_name,m_webkit_mode);
		m_chat_window->setOwnerItem(item);
		m_chat_window->setItemData(item_info,false);
		m_chat_window->setItemData(owner_info,true);
		setWindowOptions(m_chat_window);
		connectGeneralWindow(m_chat_window);
		if(!m_conference_window)
		{
		    m_merged_window.m_main_layout->addWidget(m_chat_window);
		    m_merged_window.m_main_window->setProperty("m_type",0);
		}
	    } else 
		m_chat_window->windowActivatedByUser(m_merged_window.m_main_window);

	} else
	{
	    if ( !m_conference_window)
	    {
		m_conference_window = new ConferenceWindow(m_profile_name, m_webkit_mode);
		m_conference_window->setOwnerItem(item);
		setWindowOptions(m_conference_window);
		connectGeneralWindow(m_conference_window);
		if(!m_chat_window)
		{
		    m_merged_window.m_main_layout->addWidget(m_conference_window);
		    m_merged_window.m_main_window->setProperty("m_type",32);
		}
	    } else 
		m_conference_window->windowActivatedByUser(m_merged_window.m_main_window);
	}

	m_current_window = m_merged_window;


    } else {
	if ( item.m_item_type != 32 )
	{
	    if ( !m_private_chats_window.m_main_window )
	    {

		createTabbedWindow(&m_private_chats_window);
		connect(m_private_chats_window.m_main_window, SIGNAL(destroyed(QObject*)), this, SLOT(destroyPrivatesWindow(QObject*)));
		restoreSizeAndPosition(m_private_chats_window.m_main_window,PrivatesWindow);
		m_private_chats_window.m_main_window->show();
		new_tabbed_window_was_created = true;
		m_chat_window = new ChatWindow(m_profile_name,m_webkit_mode);
		setWindowOptions(m_chat_window);
		connectGeneralWindow(m_chat_window);
		m_chat_window->setOwnerItem(item);
		m_chat_window->setItemData(item_info,false);
		m_chat_window->setItemData(owner_info,true);
		m_private_chats_window.m_main_layout->addWidget(m_chat_window);

	    } else 
		m_chat_window->windowActivatedByUser(m_private_chats_window.m_main_window);
	    m_current_window = m_private_chats_window;
	} else
	{
	    if ( !m_all_conference_window.m_main_window)
	    {
		createTabbedWindow(&m_all_conference_window);
		connect(m_all_conference_window.m_main_window, SIGNAL(destroyed(QObject*)), this, SLOT(destroyConferencesWindow(QObject*)));
		restoreSizeAndPosition(m_all_conference_window.m_main_window,ConferencesWindow);
		m_all_conference_window.m_main_window->show();
		new_tabbed_window_was_created = true;
		m_conference_window = new ConferenceWindow(m_profile_name, m_webkit_mode);
		setWindowOptions(m_conference_window);
		connectGeneralWindow(m_conference_window);
		m_conference_window->setOwnerItem(item);
		m_all_conference_window.m_main_layout->addWidget(m_conference_window);
	    } else
		m_conference_window->windowActivatedByUser(m_all_conference_window.m_main_window);
	    m_current_window = m_all_conference_window;
	}
    }
    if ( new_tabbed_window_was_created && m_remember_privates )
	emit restorePreviousTabs();
    if ( m_current_window.m_main_window )
    {
	int new_tab = -1;
	if ( item_info.count() > 0 )
	{
	    if(!new_message)
	    m_current_window.m_main_window->setWindowTitle(item_info.at(0));
	    new_tab = m_current_window.m_tab_bar->addTab(item_info.at(0));
	}
	else
	{
	    if(!new_message)
	    m_current_window.m_main_window->setWindowTitle(item.m_item_name);
	    new_tab = m_current_window.m_tab_bar->addTab(item.m_item_name);
	}

	if ( item.m_item_type == 0 )
	{
	    m_current_window.m_tab_bar->setTabIcon(new_tab,TempGlobalInstance::instance().getContactIcon(item,0));
	} else {
		m_current_window.m_tab_bar->setTabIcon(new_tab,SystemsCity::IconManager()->getIcon("chat"));
	}
	UserSpace *current_user_space = new UserSpace;
	current_user_space->m_item = item;
	current_user_space->m_identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
	if ( item.m_item_type != 32 )
	{
	    current_user_space->m_item_info = item_info;
	    current_user_space->m_owner_info = owner_info;
	}
	current_user_space->m_state = 0;
	m_current_window.m_tab_bar->setTabData(new_tab,reinterpret_cast<qptrdiff>(current_user_space));
	if(!new_message)
	{
	    m_current_window.m_tab_bar->setCurrentIndex(new_tab);
	    m_current_window.m_main_window->setWindowIcon(m_current_window.m_tab_bar->tabIcon(new_tab));
	}





    }

}


void TabbedChats::connectGeneralWindow(GeneralWindow *general_window)
{
    connect(general_window,SIGNAL(windowFocused()), this, SLOT(windowActivatedByUser()));
    connect(general_window->m_event_eater, SIGNAL(closeCurrentChat()), this, SLOT(closeCurrentChat()));
    connect(general_window->m_event_eater, SIGNAL(changeTab(int)), this, SLOT(changeTab(int)));
}

void TabbedChats::destroyMergedWindow(QObject *o)
{

    QWidget *tmp_win = qobject_cast<QWidget*>(o);
    if ( tmp_win ) saveSizeAndPosition(tmp_win, MergedWindow);

    m_chat_window = 0;
    m_conference_window = 0;

    m_merged_window.m_tab_bar = 0;
    m_merged_window.m_main_layout = 0;
    m_merged_window.m_main_window = 0;
}


void TabbedChats::destroyPrivatesWindow(QObject *o)
{

    QWidget *tmp_win = qobject_cast<QWidget*>(o);
    if ( tmp_win ) saveSizeAndPosition(tmp_win, PrivatesWindow);

    m_chat_window = 0;

    m_private_chats_window.m_tab_bar = 0;
    m_private_chats_window.m_main_layout = 0;
    m_private_chats_window.m_main_window = 0;

}

void TabbedChats::destroyConferencesWindow(QObject *o)
{

    QWidget *tmp_win = qobject_cast<QWidget*>(o);
    if ( tmp_win ) saveSizeAndPosition(tmp_win, ConferencesWindow);

    m_conference_window = 0;

    m_all_conference_window.m_tab_bar = 0;
    m_all_conference_window.m_main_layout = 0;
    m_all_conference_window.m_main_window = 0;
}

void TabbedChats::tabChanged(int index)
{
    if ( m_merge_conf_and_chats || m_merged_window.m_main_window)
    {
	if ( m_merged_window.m_main_window)
	{
	    m_merged_window.m_main_window->setWindowIcon(m_merged_window.m_tab_bar->tabIcon(index));
	    UserSpace *tmp_space = reinterpret_cast<UserSpace*>(m_merged_window.m_tab_bar->tabData(index).value<qptrdiff>());

	    if ( tmp_space )
	    {
		if ( tmp_space->m_item.m_item_type != 32 && m_merged_window.m_main_window->property("m_type").toUInt() != 0 )
		{
		    m_merged_window.m_main_layout->removeWidget(m_conference_window);
		    m_conference_window->hide();
		    m_merged_window.m_main_layout->addWidget(m_chat_window);
		    if ( !m_chat_window->isVisible() ) m_chat_window->show();

		    m_merged_window.m_main_window->setProperty("m_type",0);
		}
		else if ( tmp_space->m_item.m_item_type  == 32 && m_merged_window.m_main_window->property("m_type").toUInt() != 32 )
		{

		    m_merged_window.m_main_layout->removeWidget(m_chat_window);
		    m_chat_window->hide();
		    m_merged_window.m_main_layout->addWidget(m_conference_window);
		    if ( !m_conference_window->isVisible() ) m_conference_window->show();

		    m_merged_window.m_main_window->setProperty("m_type",32);

		}

		if ( tmp_space->m_item.m_item_type != 32 )
		{
		    m_chat_window->setOwnerItem(tmp_space->m_item);
		    m_chat_window->setItemData(tmp_space->m_item_info,false);
		    m_chat_window->setItemData(tmp_space->m_owner_info, true);
		    if (tmp_space->m_state & 0x1)
		    {
			tmp_space->m_state &= 0x10;
			TempGlobalInstance::instance().waitingItemActivated(tmp_space->m_item);
			m_merged_window.m_tab_bar->setTabIcon(index,
							      tmp_space->m_item.m_item_type == 33 ?
							  TempGlobalInstance::instance().getIcon("chat"):TempGlobalInstance::instance().getContactIcon(tmp_space->m_item,0));
			m_merged_window.m_tab_bar->setTabTextColor(index, getColorForState(tmp_space->m_state));
		    }

			m_chat_window->contactTyping(tmp_space->m_state & 0x10);


		}
		else
		{
		    m_conference_window->setOwnerItem(tmp_space->m_item);
		    m_conference_window->setItemData(tmp_space->m_owner_info, true);
		    m_merged_window.m_tab_bar->setTabTextColor(index,QColor());
		    m_merged_window.m_tab_bar->setTabText(index,tmp_space->m_item.m_item_name);
		}
		m_merged_window.m_main_window->setWindowIcon(m_merged_window.m_tab_bar->tabIcon(index));
		m_merged_window.m_main_window->setWindowTitle(m_merged_window.m_tab_bar->tabText(index));
	    }
	 }
    } else {
	UserSpace *tmp_space = reinterpret_cast<UserSpace*>(qobject_cast<QTabBar*>(sender())->tabData(index).value<qptrdiff>());
	if ( tmp_space )
	{
	    if ( tmp_space->m_item.m_item_type != 32 && m_private_chats_window.m_main_window )
	    {
		m_private_chats_window.m_main_window->setWindowIcon(m_private_chats_window.m_tab_bar->tabIcon(index));
		m_chat_window->setOwnerItem(tmp_space->m_item);
		m_chat_window->setItemData(tmp_space->m_item_info,false);
		m_chat_window->setItemData(tmp_space->m_owner_info, true);

		if (tmp_space->m_state & 0x1)
		{
			tmp_space->m_state &= 0x10;
			TempGlobalInstance::instance().waitingItemActivated(tmp_space->m_item);
			m_private_chats_window.m_tab_bar->setTabIcon(index,tmp_space->m_item.m_item_type == 33 ?
							  TempGlobalInstance::instance().getIcon("chat"):TempGlobalInstance::instance().getContactIcon(tmp_space->m_item,0));
			m_private_chats_window.m_tab_bar->setTabTextColor(index, getColorForState(tmp_space->m_state));

		}

		m_chat_window->contactTyping(tmp_space->m_state & 0x10);
		m_private_chats_window.m_main_window->setWindowIcon(m_private_chats_window.m_tab_bar->tabIcon(index));
		m_private_chats_window.m_main_window->setWindowTitle(m_private_chats_window.m_tab_bar->tabText(index));
	    }
	    else if ( tmp_space->m_item.m_item_type == 32 && m_all_conference_window.m_main_window )
	    {
		m_all_conference_window.m_main_window->setWindowIcon(m_all_conference_window.m_tab_bar->tabIcon(index));

		m_conference_window->setOwnerItem(tmp_space->m_item);
		m_conference_window->setItemData(tmp_space->m_owner_info, true);
		m_all_conference_window.m_tab_bar->setTabTextColor(index,QColor());
		m_all_conference_window.m_tab_bar->setTabText(index,tmp_space->m_item.m_item_name);
		m_all_conference_window.m_main_window->setWindowTitle(m_all_conference_window.m_tab_bar->tabText(index));
	    }
	}
    }
}

void TabbedChats::createTabbedWindow(TabbedWindow *tabbed_window)
{
    tabbed_window->m_tab_bar = new QTabBar;
    tabbed_window->m_main_window = new QWidget;
    tabbed_window->m_main_layout = new QVBoxLayout;
    tabbed_window->m_main_layout->setMargin(0);
    tabbed_window->m_tab_bar->installEventFilter(this);

    tabbed_window->m_main_layout->addWidget(tabbed_window->m_tab_bar);
    tabbed_window->m_main_window->setLayout(tabbed_window->m_main_layout);
    tabbed_window->m_main_window->setAttribute(Qt::WA_QuitOnClose, false);
    tabbed_window->m_main_window->setAttribute(Qt::WA_DeleteOnClose, true);

    connect(tabbed_window->m_tab_bar, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
    connect(tabbed_window->m_tab_bar, SIGNAL(destroyed()), this, SLOT(closeAllTabs()));
#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
//	tabbed_window->m_main_window->setAttribute(Qt::WA_TranslucentBackground, true);
    tabbed_window->m_tab_bar->setTabsClosable(m_closable_tabs);
    tabbed_window->m_tab_bar->setMovable(m_movable_tabs);
    connect(tabbed_window->m_tab_bar, SIGNAL(tabCloseRequested(int)), this, SLOT(tabClosed(int)));
#endif
}


void TabbedChats::tabClosed(int index)
{
    deleteTabData(qobject_cast<QTabBar*>(sender()), index);
}

void TabbedChats::closeAllTabs()
{
    QTabBar *tmp_bar = reinterpret_cast<QTabBar*>(sender());
    if ( tmp_bar && tmp_bar->count())
    {

	//Prepare for saving openned chats
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	QString windows_file = settings.fileName().section('/', 0, -2) + "/owindows2.list";
	QFile w_file(windows_file);
	bool yes_you_can_write_your_chat_info = w_file.open(QIODevice::WriteOnly);
	QDataStream out;
	if (yes_you_can_write_your_chat_info) out.setDevice(&w_file);

	TempGlobalInstance &tmp_tgi = TempGlobalInstance::instance();
	for( int i = 0; i < tmp_bar->count(); i++)
	{
	    UserSpace *tmp_space = reinterpret_cast<UserSpace*>(tmp_bar->tabData(i).value<qptrdiff>());
	    if ( tmp_space ){

		if ( yes_you_can_write_your_chat_info && tmp_space->m_item.m_item_type != 32)
		    out << tmp_space->m_item.m_protocol_name
					<< tmp_space->m_item.m_account_name
					<< tmp_space->m_item.m_parent_name
					<< tmp_space->m_item.m_item_name
					<< tmp_space->m_item.m_item_type;
		m_logs_city.destroyMyHome(tmp_space->m_item,true);
		tmp_tgi.chatClosed(tmp_space->m_item);
		delete tmp_space;

	    }
	}
	if (yes_you_can_write_your_chat_info) w_file.close();
    }
}

void TabbedChats::deleteTabData(QTabBar *tab_bar,int index)
{
    if ( index < 0 || index >= tab_bar->count() )
		return;
    UserSpace *tmp_space = reinterpret_cast<UserSpace*>(tab_bar->tabData(index).value<qptrdiff>());
    if ( tmp_space )
    {
	TreeModelItem tmp_item = tmp_space->m_item;
	delete tmp_space;
	tab_bar->removeTab(index);
	if ( !tab_bar->count() )
	{
	    if ( m_merge_conf_and_chats || m_merged_window.m_main_window)
		m_merged_window.m_main_window->close();
	    else {
		if ( tmp_item.m_item_type !=32 && m_private_chats_window.m_main_window)
		    m_private_chats_window.m_main_window->close();
		else
		if ( tmp_item.m_item_type == 32 && m_all_conference_window.m_main_window)
		    m_all_conference_window.m_main_window->close();
	    }
	} else if ( m_merge_conf_and_chats || m_merged_window.m_main_window )
	{
		bool i_am_last_item_for_my_type = true;
		bool i_am_conference = tmp_item.m_item_type == 32;
		for(int i = 0; i < tab_bar->count();i++)
		{
			UserSpace *tmp_space = reinterpret_cast<UserSpace*>(tab_bar->tabData(i).value<qptrdiff>());
			if(!tmp_space)
				continue;
			bool it_is_conference = tmp_space->m_item.m_item_type == 32;
			if ( i_am_conference == it_is_conference)
				i_am_last_item_for_my_type = false;
		}

		if (i_am_last_item_for_my_type)
		{
			if ( tmp_item.m_item_type != 32 )
			{
				m_chat_window->close();
				m_chat_window = 0;
			} else {
				m_conference_window->close();
				m_conference_window = 0;
			}
		}
	}
	m_logs_city.destroyMyHome(tmp_item, !tab_bar->count());
	TempGlobalInstance::instance().chatClosed(tmp_item);
    }
}


QTabBar *TabbedChats::getBarForItemType(int type)
{
    if ( m_merge_conf_and_chats || m_merged_window.m_tab_bar)
	return m_merged_window.m_tab_bar;
    else {
	if ( type !=32  && m_private_chats_window.m_tab_bar)
	    return m_private_chats_window.m_tab_bar;
	else
	if ( type == 32 && m_all_conference_window.m_tab_bar)
	    return m_all_conference_window.m_tab_bar;
    }
	// FIXME: There were no return, should it be NULL?
	return 0;
}

QWidget *TabbedChats::getMainWindowForItemType(int type)
{
    if ( m_merge_conf_and_chats || m_merged_window.m_main_window)
	return m_merged_window.m_main_window;
    else {
	if ( type !=32 && m_private_chats_window.m_main_window)
	    return m_private_chats_window.m_main_window;
	else
	if ( type == 32 && m_all_conference_window.m_main_window)
	    return m_all_conference_window.m_main_window;
    }
	// FIXME: There were no return, should it be NULL?
	return 0;
}

GeneralWindow *TabbedChats::getGeneralWindowForItemType(int type)
{
	if ( type != 32)
	    return m_chat_window;
	else
	    return m_conference_window;
}

void TabbedChats::activateWindow(const TreeModelItem &item,bool new_message)
{

    QTabBar *tmp_bar = getBarForItemType(item.m_item_type);
    QWidget *tmp_main_window = getMainWindowForItemType(item.m_item_type);


    if ( m_chat_window ) m_chat_window->windowActivatedByUser(tmp_main_window);
    else if ( m_conference_window ) m_conference_window->windowActivatedByUser(tmp_main_window);

    if ( tmp_bar && !new_message)
    {
	for(int i = 0; i < tmp_bar->count();i++)
	{
	    UserSpace *tmp_space = reinterpret_cast<UserSpace*>(tmp_bar->tabData(i).value<qptrdiff>());
	    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
		.arg(item.m_account_name).arg(item.m_item_name);
	    if ( tmp_space && tmp_space->m_identification == identification){
		tmp_bar->setCurrentIndex(i);
		return;
	    }
	}
    }
}

void TabbedChats::contactChangeHisStatus(const TreeModelItem &item, const QIcon &icon)
{
	QTabBar *tmp_bar = getBarForItemType(item.m_item_type);
	QWidget *tmp_main_window = getMainWindowForItemType(item.m_item_type);
	QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
			.arg(item.m_account_name).arg(item.m_item_name);
	if ( tmp_bar )
	{
		for(int i = 0; i < tmp_bar->count();i++)
		{
			UserSpace *tmp_space = reinterpret_cast<UserSpace*>(tmp_bar->tabData(i).value<qptrdiff>());
			if ( tmp_space && tmp_space->m_identification == identification)
			{
				tmp_bar->setTabIcon(i,icon);
				if ( tmp_bar->currentIndex() == i)
					tmp_main_window->setWindowIcon(icon);
				return;
			}
		}
	}
}

void TabbedChats::contactChangeHisClient(const TreeModelItem &item)
{
	QTabBar *tmp_bar = getBarForItemType(item.m_item_type);
	GeneralWindow *tmp_general = getGeneralWindowForItemType(item.m_item_type);
	QWidget *tmp_main_window = getMainWindowForItemType(item.m_item_type);
	QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
			.arg(item.m_account_name).arg(item.m_item_name);
	if ( tmp_bar )
	{
		for(int i = 0; i < tmp_bar->count();i++)
		{
			UserSpace *tmp_space = reinterpret_cast<UserSpace*>(tmp_bar->tabData(i).value<qptrdiff>());
			if ( tmp_space && tmp_space->m_identification == identification)
			{
				tmp_space->m_item_info = SystemsCity::PluginSystem()->getAdditionalInfoAboutContact(item);
				if(tmp_bar->currentIndex() == i && tmp_general)
					tmp_general->setItemData(tmp_space->m_item_info, false);
				return;
			}
		}
	}
}

bool TabbedChats::checkForActivation(const TreeModelItem &item, bool just_check)
{
    int tmp_type = item.m_item_type == 34 ? 32 : item.m_item_type;
    QTabBar *tmp_bar = getBarForItemType(tmp_type);
    QWidget *tmp_main_window = getMainWindowForItemType(tmp_type);
    GeneralWindow *tmp_general = getGeneralWindowForItemType(tmp_type);
	QString identification;
	if ( item.m_item_type != 34)
	 identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
	else
	identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_parent_name);
    if ( tmp_bar )
    {
	UserSpace *tmp_space = reinterpret_cast<UserSpace*>(tmp_bar->tabData(tmp_bar->currentIndex()).value<qptrdiff>());
	if (tmp_general && tmp_space && tmp_space->m_identification == identification) tmp_general->checkForScrollBarMaximum();
	if (tmp_space && tmp_space->m_identification == identification && tmp_main_window
	    && !(!tmp_main_window->isActiveWindow() || tmp_main_window->isMinimized()))
	return false;

	if ( just_check ) return false;

	if ( tmp_general && !tmp_general->m_waiting_for_activation )
	{
	    if( item.m_item_type != 32 && item.m_item_type != 34 && !m_dont_blink)
		qApp->alert(tmp_main_window, 0);
	    tmp_general->m_waiting_for_activation = true;
	}


	for(int i = 0; i < tmp_bar->count();i++)
	{
	    UserSpace *tmp_space = reinterpret_cast<UserSpace*>(tmp_bar->tabData(i).value<qptrdiff>());
	    if ( tmp_space && tmp_space->m_identification == identification){
		if(item.m_item_type != 32 && item.m_item_type != 34)
		{
		    tmp_space->m_state |= 0x01;
		    tmp_bar->setTabTextColor(i,getColorForState(tmp_space->m_state));
		    tmp_bar->setTabIcon(i,TempGlobalInstance::instance().getIcon("message"));
		} else
		{
		    tmp_bar->setTabText(i,"*" +( item.m_item_type == 34?item.m_parent_name:item.m_item_name));
		    if ( tmp_bar->currentIndex() == i) tmp_main_window->setWindowTitle("*" + ( item.m_item_type == 34?item.m_parent_name:item.m_item_name));
		}
	    }
	}
	return true;
    }
    return false;
}

QColor TabbedChats::getColorForState(quint8 state)
{
	if ( state & 0x01 )
		return QColor(Qt::red);
	else if (state & 0x10)
		return QColor(Qt::darkGreen);
	else
		return QColor();
}

void TabbedChats::windowActivatedByUser()
{
    GeneralWindow *tmp_win = qobject_cast<GeneralWindow*>(sender());
    if ( tmp_win )
    {
	TreeModelItem item = tmp_win->m_item;

	QTabBar *tmp_bar = getBarForItemType(item.m_item_type);
	if ( tmp_bar )
	{
	    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	    .arg(item.m_account_name).arg(item.m_item_name);
	    int current_tab = tmp_bar->currentIndex();
	    UserSpace *tmp_space = reinterpret_cast<UserSpace*>(tmp_bar->tabData(current_tab).value<qptrdiff>());
	    if ( tmp_space && tmp_space->m_identification == identification )
	    {

		if ( item.m_item_type != 32 )
		{
		    TempGlobalInstance::instance().waitingItemActivated(item);
			tmp_bar->setTabIcon(current_tab,tmp_space->m_item.m_item_type == 33
								? TempGlobalInstance::instance().getIcon("chat")
								: TempGlobalInstance::instance().getContactIcon(tmp_space->m_item,0));
		    tmp_space->m_state &= 0x10;
		    tmp_bar->setTabTextColor(current_tab,getColorForState(tmp_space->m_state));
		}
		else
		{
		    QWidget *tmp_main = getMainWindowForItemType(32);
		    tmp_main->setWindowTitle(item.m_item_name);
		    tmp_bar->setTabText(current_tab,item.m_item_name);
		    LogsCity::instance().notifyAboutFocusingConference(item);
		}
	    }
	}
    }

}

void TabbedChats::setItemTypingState(const TreeModelItem &item, TypingAttribute state)
{

    if(item.m_item_type != 32 )
    {
	QTabBar *tmp_bar = getBarForItemType(item.m_item_type);

	if(tmp_bar)
	{
	    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
		    .arg(item.m_account_name).arg(item.m_item_name);
	    for(int i = 0; i < tmp_bar->count();i++)
	    {
		UserSpace *tmp_space = reinterpret_cast<UserSpace*>(tmp_bar->tabData(i).value<qptrdiff>());
		if ( tmp_space && tmp_space->m_identification == identification){


			if ( (bool)state ) tmp_space->m_state |= 0x10;
			else tmp_space->m_state &= 0x1;


		    tmp_bar->setTabTextColor(i,getColorForState(tmp_space->m_state));
		if (tmp_bar->currentIndex() == i )
		{
		    GeneralWindow *tmp_win = getGeneralWindowForItemType(item.m_item_type);
		    if(tmp_win) tmp_win->contactTyping((bool)state);
		}
		}
	    }
	}
    }
}

void TabbedChats::changeId(const TreeModelItem &item, const QString &new_id)
{
    QTabBar *tmp_bar = getBarForItemType(item.m_item_type);
    GeneralWindow *tmp_general = getGeneralWindowForItemType(item.m_item_type);
	QString identification = QString("%1.%2.%3").arg(item.m_protocol_name, item.m_account_name, item.m_item_name);
	QString new_identification = QString("%1.%2.%3").arg(item.m_protocol_name, item.m_account_name, new_id);
	UserSpace *space = 0;
	int space_i = -1;
    if ( tmp_bar )
    {
		for(int i = 0; i < tmp_bar->count();i++)
		{

			UserSpace *tmp_space = reinterpret_cast<UserSpace*>(tmp_bar->tabData(i).value<qptrdiff>());
			if ( tmp_space && tmp_space->m_identification == identification)
			{
				space = tmp_space;
				space_i = i;
			}
			else if ( tmp_space && tmp_space->m_identification == new_identification)
				return;
		}
    }
	if( space )
	{
		space->m_identification = new_identification;
		space->m_item.m_item_name = new_id;
		if ( tmp_bar->currentIndex() == space_i && tmp_general)
			tmp_general->m_item.m_item_name = new_id;
		LogsCity::instance().moveMyHome( identification, new_identification );
	}
}

void TabbedChats::closeCurrentChat()
{
    EventEater *tmp_eater = qobject_cast<EventEater*>(sender());
    if ( tmp_eater )
    {
	QTabBar *tmp_bar = getBarForItemType(tmp_eater->m_item.m_item_type);
	if ( tmp_bar )
	    deleteTabData(tmp_bar,tmp_bar->currentIndex());
    } else {

	GeneralWindow *tmp_win = qobject_cast<GeneralWindow*>(sender());
	if ( tmp_win )
	{
	    QTabBar *tmp_bar = getBarForItemType(tmp_win->m_item.m_item_type);
	    if ( tmp_bar )
		deleteTabData(tmp_bar,tmp_bar->currentIndex());
	}
    }
}

void TabbedChats::changeTab(int offset)
{
    EventEater *tmp_eater = qobject_cast<EventEater*>(sender());
    if ( tmp_eater )
    {
	QTabBar *tmp_bar = getBarForItemType(tmp_eater->m_item.m_item_type);
	if ( tmp_bar )
	{
	    int cur_tab = tmp_bar->currentIndex();
	    cur_tab+=offset;
	    if(cur_tab<0) cur_tab = tmp_bar->count()-1;
	    if(cur_tab>=tmp_bar->count()) cur_tab = 0;
	    tmp_bar->setCurrentIndex(cur_tab);
	}
    }
}
bool TabbedChats::eventFilter(QObject *obj, QEvent *event)
{
	if( event->type() == QEvent::MouseButtonDblClick
		|| ( event->type() == QEvent::MouseButtonRelease
			&& ( static_cast<QMouseEvent *>(event)->button() == Qt::MidButton ) ) )
	{
		if ( QTabBar *tmp_tab = qobject_cast<QTabBar*>(obj) )
		{
			int tab_index = tmp_tab->tabAt(static_cast<QMouseEvent*>(event)->pos());
			if (tab_index > -1)
				deleteTabData(tmp_tab,tab_index);
		}
	}
	else if( event->type() == QEvent::ContextMenu )
	{
		if ( QTabBar *tmp_tab = qobject_cast<QTabBar*>(obj) )
		{
			QPoint pos = static_cast<QMouseEvent*>(event)->pos();
			int tab_index = tmp_tab->tabAt(pos);
			if ( tab_index < 0 || tab_index >= tmp_tab->count() )
				return QObject::eventFilter(obj, event);
			pos = tmp_tab->mapToGlobal( pos );
			UserSpace *tmp_space = reinterpret_cast<UserSpace*>(tmp_tab->tabData(tab_index).value<qptrdiff>());
			if( tmp_space->m_item.m_item_type == 32 )
			{
				TempGlobalInstance::instance().showConferenceMenu(tmp_space->m_item,pos);
			}
			else
			{
				Q_REGISTER_EVENT( context_menu, "Core/ContactList/ContextMenu" );
				if( tmp_space )
					Event( context_menu, 2, &tmp_space->m_item, &pos ).send();
			}
		}
	}
	return QObject::eventFilter(obj, event);
}

void TabbedChats::saveSizeAndPosition(QWidget *win, TabbedWindowType type)
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	switch(type)
	{
	    case MergedWindow: settings.beginGroup("merged");break;
	    case ConferencesWindow: settings.beginGroup("conference");break;
	    default:settings.beginGroup("chatwindow");
	}
	settings.setValue("position", win->pos());
	settings.setValue("size", win->size());
	settings.endGroup();
}

void TabbedChats::restoreSizeAndPosition(QWidget *win, TabbedWindowType type)
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	switch(type)
	{
	    case MergedWindow: settings.beginGroup("merged");break;
	    case ConferencesWindow: settings.beginGroup("conference");break;
	    default:settings.beginGroup("chatwindow");
	}
	win->move(settings.value("position", QPoint(0,0)).toPoint());
	win->resize(settings.value("size", QSize(400,300)).toSize());
	settings.endGroup();
}

void TabbedChats::setWindowOptions(GeneralWindow *win)
{
    win->setOptions(m_close_after_send, m_send_on_enter, m_send_on_double_enter,m_send_typing_notifications);
    connect(win, SIGNAL(closeMe()), this, SLOT(closeCurrentChat()));
}

void TabbedChats::alertWindow(const TreeModelItem &item)
{
    if ( m_conference_window )
    {
	QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_parent_name);
	QWidget *tmp_win = getMainWindowForItemType(32);
	GeneralWindow *tmp_general = getGeneralWindowForItemType(32);
	QTabBar *tmp_bar = getBarForItemType(32);
	if (tmp_bar)
	{
	    for(int i = 0; i < tmp_bar->count();i++)
	    {

		UserSpace *tmp_space = reinterpret_cast<UserSpace*>(tmp_bar->tabData(i).value<qptrdiff>());

		if ( tmp_space && tmp_space->m_identification == identification && (i != tmp_bar->currentIndex())){
			tmp_bar->setTabTextColor(i,Qt::red);
		}

	    }
	}
	qApp->alert(tmp_win, 0);
    }
}

void TabbedChats::setConferenceTopic(const TreeModelItem &item, const QString &topic)
{
    if ( m_conference_window )
    {
	QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);

	QWidget *tmp_win = getMainWindowForItemType(32);
	GeneralWindow *tmp_general = getGeneralWindowForItemType(32);
	QTabBar *tmp_bar = getBarForItemType(32);
	if (tmp_bar)
	{
	    for(int i = 0; i < tmp_bar->count();i++)
	    {

		UserSpace *tmp_space = reinterpret_cast<UserSpace*>(tmp_bar->tabData(i).value<qptrdiff>());

		if ( tmp_space && tmp_space->m_identification == identification){

		    QStringList tmp;
		    tmp<<topic;
		    tmp_space->m_owner_info = tmp;
		    if ( i == tmp_bar->currentIndex() )
			tmp_general->setItemData(tmp,true);
		}

	    }
	}
	qApp->alert(tmp_win, 0);
    }
}

QTextEdit *TabbedChats::getEditField(const TreeModelItem &item)
{
	GeneralWindow *window = getGeneralWindowForItemType(item.m_item_type);
	if(window && window->compareItem(item))
		return window->getEditField();
	return 0;
}

void TabbedChats::updateWebkit(bool update)
{
if (m_chat_window) m_chat_window->updateWebkit(update);
if (m_conference_window) m_conference_window->updateWebkit(update);
}

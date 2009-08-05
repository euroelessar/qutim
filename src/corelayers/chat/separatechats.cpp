/*
  SeparateChats

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

#include "separatechats.h"

SeparateChats::SeparateChats(const QString &profile_name) : m_profile_name(profile_name),
m_logs_city(LogsCity::instance())
{

}

SeparateChats::~SeparateChats()
{
    qDeleteAll(m_chat_list);
}

void SeparateChats::loadSettings()
{
    QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
    settings.beginGroup("chatwindow");
    m_webkit_mode = settings.value("webkit",true).toBool();
    m_close_after_send = settings.value("close", false).toBool();
    m_send_on_double_enter = settings.value("ondoubleenter", false).toBool();
	m_send_on_enter = m_send_on_double_enter || settings.value("onenter", false).toBool();
    m_send_typing_notifications = settings.value("typing", true).toBool();
    m_dont_blink = settings.value("dontblink", false).toBool();
    settings.endGroup();

}

void SeparateChats::createChat(const TreeModelItem &item,const QStringList &item_info,const QStringList &owner_info)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);

    m_logs_city.createHomeForMe(item, m_webkit_mode, item_info.count() > 0 ? item_info.at(0) : item.m_item_name,
				owner_info.count() > 0 ? owner_info.at(0) : item.m_account_name,
				owner_info.count() > 1 ? owner_info.at(1) : "",
				item_info.count() > 1 ? item_info.at(1) : "");

    if ( item.m_item_type != 32 )
    {
	ChatWindow *win = new ChatWindow(m_profile_name, m_webkit_mode);
	win->setOwnerItem(item);
	win->setWindowTitle(item.m_item_name);
	connect(win, SIGNAL(destroyed(QObject*)), this, SLOT(chatClosed(QObject*)));
	m_chat_list.insert(identification, win);
	connect(win,SIGNAL(windowFocused()), this, SLOT(windowActivatedByUser()));
	connect(win->m_event_eater, SIGNAL(closeCurrentChat()), win, SLOT(close()));
	if ( item.m_item_type == 0 )
	    win->setWindowIcon(TempGlobalInstance::instance().getContactIcon(item,0));
	else
	    win->setWindowIcon(TempGlobalInstance::instance().getIcon("chat"));

	 if ( item_info.count() > 0 )
		win->setWindowTitle(item_info.at(0));
	win->setItemData(item_info, false);
	win->setItemData(owner_info, true);
	restoreWindowSizeAndPosition(win);
	setWindowOptions(win);
	win->show();
    } else
    {
	ConferenceWindow *win = new ConferenceWindow(m_profile_name, m_webkit_mode);
	win->setOwnerItem(item);
	win->setWindowTitle(item.m_item_name);
	connect(win, SIGNAL(destroyed(QObject*)), this, SLOT(chatClosed(QObject*)));
	connect(win,SIGNAL(windowFocused()), this, SLOT(windowActivatedByUser()));
	connect(win->m_event_eater, SIGNAL(closeCurrentChat()), win, SLOT(close()));
	m_chat_list.insert(identification, win);
	win->setWindowIcon(TempGlobalInstance::instance().getIcon("chat"));
	restoreWindowSizeAndPosition(win);
	setWindowOptions(win);
	win->show();
    }

}

void SeparateChats::chatClosed(QObject *win)
{
    GeneralWindow *w = reinterpret_cast<GeneralWindow*>(win);

    if ( w )
    {
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup((w->m_item.m_item_type != 32)?"chatwindow":"conference");
	settings.setValue("position", w->pos());
	settings.setValue("size", w->size());
	settings.endGroup();

	LogsCity::instance().destroyMyHome(w->m_item);
	m_chat_list.remove(m_chat_list.key(w));
	TempGlobalInstance::instance().chatClosed(w->m_item);


    }
}


void SeparateChats::restoreWindowSizeAndPosition(GeneralWindow *win)
{
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+m_profile_name, "profilesettings");
	settings.beginGroup((win->m_item.m_item_type != 32)?"chatwindow":"conference");
	win->move(settings.value("position", QPoint(0,0)).toPoint());
	win->resize(settings.value("size", QSize(400,300)).toSize());
	settings.endGroup();
}

void SeparateChats::activateWindow(const TreeModelItem &item)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
    if ( m_chat_list.contains(identification) )
	m_chat_list.value(identification)->windowActivatedByUser(qobject_cast<QWidget*>(m_chat_list.value(identification)));
}

void SeparateChats::contactChangeHisStatus(const TreeModelItem &item, const QIcon &icon)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
	if ( GeneralWindow *window = m_chat_list.value(identification, 0) )
		window->setWindowIcon(icon);
}

void SeparateChats::contactChangeHisClient(const TreeModelItem &item)
{
	QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
	if ( GeneralWindow *window = m_chat_list.value(identification, 0) )
	{
		QStringList info = SystemsCity::PluginSystem()->getAdditionalInfoAboutContact(item);
		window->setItemData(info, false);
	}
}

bool SeparateChats::checkForActivation(const TreeModelItem &item, bool just_check)
{
    QString identification;
	if ( item.m_item_type != 34)
	 identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
	else
	identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_parent_name);
     if ( m_chat_list.contains(identification) )
    {
    	GeneralWindow *tmp_win = m_chat_list.value(identification);
	tmp_win->checkForScrollBarMaximum();

	if ( (!tmp_win->isActiveWindow() || tmp_win->isMinimized()) )
	{

	    if ( !tmp_win->m_waiting_for_activation && !just_check)
	    {
	    if ( item.m_item_type != 32 && item.m_item_type != 34)
		{
		    if ( !m_dont_blink ) qApp->alert(tmp_win, 0);
		}
		else
		    tmp_win->setWindowTitle("*"+tmp_win->windowTitle());
		tmp_win->m_waiting_for_activation = true;
	    }
	    return true;
	}
    }
     return false;
}

void SeparateChats::windowActivatedByUser()
{
    GeneralWindow *tmp_win = qobject_cast<GeneralWindow*>(sender());

    if ( tmp_win )
    {

	TreeModelItem tmp_item = tmp_win->m_item;
	if ( tmp_item.m_item_type != 32 )
	    TempGlobalInstance::instance().waitingItemActivated(tmp_item);
	else
	{
	    tmp_win->setWindowTitle(tmp_item.m_item_name);
	    LogsCity::instance().notifyAboutFocusingConference(tmp_item);
	}
    }

}

void SeparateChats::setItemTypingState(const TreeModelItem &item, TypingAttribute state)
{

    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);

    if ( m_chat_list.contains(identification) )
	m_chat_list.value(identification)->contactTyping((bool)state);
}

void SeparateChats::changeId(const TreeModelItem &item, const QString &new_id)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);
	QString new_identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(new_id);
	if ( m_chat_list.contains(identification) && !m_chat_list.contains(new_identification) )
    {
		GeneralWindow *tmp_win = m_chat_list.value(identification);
		m_chat_list.remove(identification);
		LogsCity::instance().moveMyHome( identification, new_identification );
		m_chat_list.insert(new_identification,tmp_win);
		tmp_win->m_item.m_item_name = new_id;
    }
}

void SeparateChats::setWindowOptions(GeneralWindow *win)
{
    win->setOptions(m_close_after_send,m_send_on_enter, m_send_on_double_enter,m_send_typing_notifications);
    connect(win, SIGNAL(closeMe()), this, SLOT(closeChat()));
}

void SeparateChats::closeChat()
{
    GeneralWindow *tmp_win = qobject_cast<GeneralWindow*>(sender());
    if ( tmp_win ) tmp_win->close();
}

void SeparateChats::alertWindow(const TreeModelItem &item)
{
    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_parent_name);
    if ( m_chat_list.contains(identification)  )
    {

	if (!m_dont_blink) qApp->alert(m_chat_list.value(identification), 0);
	m_chat_list.value(identification)->checkForScrollBarMaximum();
    }
}

void SeparateChats::setConferenceTopic(const TreeModelItem &item, const QString &topic)
{

    QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
	.arg(item.m_account_name).arg(item.m_item_name);

    if ( m_chat_list.contains(identification)  )
    {
	QStringList tmp;
	tmp<<topic;
	m_chat_list.value(identification)->setItemData(tmp,true);
    }
}

QTextEdit *SeparateChats::getEditField(const TreeModelItem &item)
{
	QString identification = QString("%1.%2.%3").arg(item.m_protocol_name)
							 .arg(item.m_account_name).arg(item.m_item_name);
	GeneralWindow *window = m_chat_list.value(identification, 0);
	return window ? window->getEditField() : 0;
}

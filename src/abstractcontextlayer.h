/*
    AbstractContextLayer

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef ABSTRACTCONTEXTLAYER_H_
#define ABSTRACTCONTEXTLAYER_H_

#include "pluginsystem.h"

class AbstractContextLayer : public QObject, public EventHandler
{
	Q_OBJECT
public:
	static AbstractContextLayer &instance();
	void itemContextMenu(const TreeModelItem &item, const QPoint &menu_point);
	void conferenceItemContextMenu(const QString &protocol_name, const QString &conference_name,
			const QString &account_name, const QString &nickname, const QPoint &menu_point);
	void registerContactMenuAction(QAction *plugin_action);
	void createActions();
	void processEvent(Event &event);
private slots:
	void openChatWindowAction();
	void openHistoryAction();
	void openInformationAction();
	void openConferenceChatWindowAction();
//	void openConferenceHistoryAction();
	void openConferenceInformationAction();

private:
	AbstractContextLayer();
	virtual ~AbstractContextLayer();
	QAction *m_open_chat_action;
	QAction *m_open_hisory_action;
	QAction *m_open_info_action;
	QAction *m_open_conference_chat_action;
//	QAction *m_open_conference_hisory_action;
	QAction *m_open_conference_info_action;
	QList<QAction*> m_actions_list;
	QList<QAction*> m_conference_actions_list;
	TreeModelItem m_current_item;
	QString m_current_protocol;
	QString m_current_conference;
	QString m_current_account;
        QString m_current_nickname;
        quint16 m_contact_menu;
        quint16 m_conference_menu;
};

#endif /*ABSTRACTCONTEXTLAYER_H_*/

/*
    Conference Contact List

    Copyright (c) 2008 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef CONFERENCECONTACTLIST_H_
#define CONFERENCECONTACTLIST_H_

#include <QListView>
#include <QString>
#include <QIcon>
#include "../contactlist/contactlistitemdelegate.h"
#include "conferenceitemmodel.h"


class ConfContactList;
class ConferenceItemModel;


class ConfContactListEventEater : public QObject
{
	Q_OBJECT
public:
	ConfContactListEventEater();
	ConfContactList *m_contact_list;
protected:
	bool eventFilter(QObject *obj, QEvent *event);
public slots:
	void itemActivated(const QModelIndex & index);
};

class ConfContactList
{
public:
	ConfContactList(const QString &protocol_name, const QString &conference_name, const QString &account_name,QListView *list_view);
	virtual ~ConfContactList();
	void addConferenceItem(const QString &nickname);
	void removeConferenceItem(const QString &nickname);
	void renameConferenceItem(const QString &nickname, const QString &new_nickname);
	void setConferenceItemStatus(const QString &nickname, const QIcon &icon, const QString &status, int mass);
	void setConferenceItemIcon(const QString &nickname,	const QIcon &icon, int position);
	void setConferenceItemRole(const QString &nickname, const QIcon &icon, const QString &role, int mass);
	void sendEventActivated(const QModelIndex & index);
	void sendEventClicked(const QModelIndex & index, const QPoint & point);
	QString getToolTip(const QString &nickname);
	QStringList getUsers();
	void nowActive();
private:
	//PluginSystem &m_plugin_system;
	QString m_protocol_name;
	QString m_conference_name;
	QString m_account_name;
	QListView *m_list_view;
	ConferenceItemModel *m_item_model;
};

#endif /*CONFERENCECONTACTLIST_H_*/

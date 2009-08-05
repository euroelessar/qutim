/*
    Conference Item

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

#ifndef CONFERENCEITEM_H_
#define CONFERENCEITEM_H_
#include <QList>
#include <QVariant>
#include <QVector>
#include <QHash>
#include <QIcon>
#include <QPainter>
#include "confcontactlist.h"

class ConfContactList;

class ConferenceItem
{
public:
	ConferenceItem(const QVariant & display, ConfContactList *contact_list);
	virtual ~ConferenceItem();
	QVariant data(int role) const;
	bool setData(const QVariant &value, int role);
	void setImage(QIcon icon, int column);
	QIcon getImage(int column);
	void setRow(QVariant item, int row);
	void setStatus(QString text, QIcon icon, int mass);
	void setRole(QString text, QIcon icon, int mass);
	int getMass();
private:
	ConfContactList *m_contact_list;
	QVariant m_item_display;
        QList<QVariant> m_item_icons;
        QList<QVariant> m_item_bottom_rows;
	QVariant m_item_type;
	int m_item_status_mass;
	int m_item_role_mass;
	QString m_item_status;
	QString m_item_role;
	QVariant m_current_status_icon;
};

#endif /*CONFERENCEITEM_H_*/

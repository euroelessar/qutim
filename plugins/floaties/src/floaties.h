/*
    Floaties

    Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef FLOATIES_H
#define FLOATIES_H

#include "contactwidget.h"
#include "floatiesitemmodel.h"
#include <qutim/plugin.h>
#include <qutim/contact.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <QAbstractItemView>

using namespace qutim_sdk_0_3;

class FloatiesPlugin : public Plugin
{
	Q_OBJECT
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
	bool eventFilter(QObject *obj, QEvent *ev);
public slots:
	void onRemoveContact(QObject *obj);
protected:
	ContactWidget *createWidget(qutim_sdk_0_3::Contact *contact);
private:
	QPointer<QAbstractItemView> m_view;
	quint16 m_eventId;
	FloatiesItemModel *m_model;
	QMap<Contact*, ContactWidget*> m_contacts;
	ActionGenerator *m_action;
};

#endif // FLOATIES_H

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
	Q_PLUGIN_METADATA(IID "org.qutim.Plugin")
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


/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "manager.h"
#include <QStandardItemModel>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/icon.h>
#include <qutim/contact.h>
#include <qutim/message.h>
#include <QTime>

namespace MassMessaging
{
class MessagingItem : public QStandardItem
{
public:
	MessagingItem(const QString &title) : QStandardItem(title)
	{
		setCheckable(true);
		setEditable(false);
	}
	virtual void setData(const QVariant& value, int role = Qt::UserRole + 1)
	{
		switch (role) {
		case Qt::CheckStateRole: {
			for (int i=0;i!=rowCount();i++)
				child(i,0)->setData(value,role);
			break;
		}
		default:
			break;
		}
		QStandardItem::setData(value,role);
	}
};

Manager::Manager(QObject* parent): QObject(parent)
{
	m_model =  new QStandardItemModel(this);
}

QAbstractItemModel* Manager::model() const
{
	return m_model;
}
void Manager::reload()
{
	m_model->clear();
	m_receivers.clear();
	m_contacts.clear();
	foreach(Protocol *proto, Protocol::all()) {
		QStandardItem *proto_item = new MessagingItem(proto->id());
		proto_item->setIcon(Icon("applications-internet"));
		foreach(Account *account, proto->accounts()) {
			QStandardItem *account_item = new MessagingItem (account->id());
			account_item->setToolTip(account->name());
			account_item->setIcon(Icon("applications-internet"));
			foreach (Contact *contact, account->findChildren<Contact *>()) {
				QStandardItem *contact_item = new MessagingItem (contact->title());
				contact_item->setIcon(contact->status().icon());
				contact_item->setData(qVariantFromValue(contact),Qt::UserRole);

				account_item->appendRow(contact_item);
				m_contacts.append(contact_item);
			}
			
			proto_item->appendRow(account_item);
		}
		if (proto_item->columnCount())
			m_model->appendRow(proto_item);
		else
			delete proto_item;
	}
}

QString Manager::parseText(const QString& msg, Contact* c)
{
	QString parsed_message = msg;
	parsed_message.replace("{receiver}",c->title());
	parsed_message.replace("{sender}",c->account()->name());
	parsed_message.replace("{time}",QTime::currentTime().toString());
	return parsed_message;
}


void Manager::start(const QString &message, int interval)
{
	m_message = message;
	foreach (QStandardItem *item, m_contacts) {
		Qt::CheckState state = static_cast<Qt::CheckState>(item->data(Qt::CheckStateRole).value<int>());
		if (state == Qt::Checked)
			m_receivers.enqueue(item);
	}
	m_total_item_count = m_receivers.count();
	if (m_receivers.count()) {
		m_timer.start(interval, this);
		QTimerEvent ev(m_timer.timerId());
		timerEvent(&ev);
	}
	else
		emit finished(false);

}
void Manager::stop()
{
	m_timer.stop();
	m_receivers.clear();
	emit finished(true);
}
Manager::~Manager()
{

}

void Manager::timerEvent(QTimerEvent* ev)
{
	if (ev->timerId() == m_timer.timerId()) {
		if (m_receivers.count()) {
			QStandardItem *item = m_receivers.dequeue();
			if (Contact *c = item->data(Qt::UserRole).value<Contact *>()) {
				Message msg (parseText(m_message,c));
				c->sendMessage(msg);
				emit update(m_total_item_count - m_receivers.count(),
							m_total_item_count,
							c->title());
			}
			if (!m_receivers.count())
				stop();
		}
		else
			stop();
	}
	QObject::timerEvent(ev);
}

bool Manager::currentState()
{
	return m_timer.isActive();
}


}


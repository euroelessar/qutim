/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "contactlistfrontmodel.h"
#include <qutim/protocol.h>
#include <QtDebug>

using namespace qutim_sdk_0_3;

ContactListFrontModel::ContactListFrontModel(QObject *parent) :
	QSortFilterProxyModel(parent), m_showOffline(true)
{
	Config config;
	config.beginGroup("contactList");
	m_showOffline = config.value("showOffline", true);

	sort(0);
	setDynamicSortFilter(true);
	onServiceChanged(m_model.name(), m_model, NULL);
	connect(ServiceManager::instance(), SIGNAL(serviceChanged(QByteArray,QObject*,QObject*)),
			this, SLOT(onServiceChanged(QByteArray,QObject*,QObject*)));
}

bool ContactListFrontModel::offlineVisibility() const
{
	return m_showOffline;
}

void ContactListFrontModel::setOfflineVisibility(bool showOffline)
{
	if (m_showOffline != showOffline) {
		Config config;
		config.beginGroup("contactList");
		config.setValue("showOffline", showOffline);
		m_showOffline = showOffline;
		emit offlineVisibilityChanged(showOffline);
		invalidateFilter();
	}
}

QStringList ContactListFrontModel::tags() const
{
	return m_model->tags();
}

QStringList ContactListFrontModel::filterTags() const
{
	return m_filterTags;
}

void ContactListFrontModel::setFilterTags(const QStringList &filterTags)
{
	if (m_filterTags == filterTags)
		return;
	m_filterTags = filterTags;
	emit filterTagsChanged(m_filterTags);
	invalidateFilter();
}

void ContactListFrontModel::inverseOfflineVisibility()
{
	setOfflineVisibility(!m_showOffline);
}

void ContactListFrontModel::addContact(Contact *contact)
{
	static_cast<ContactListBaseModel*>(sourceModel())->onContactAdded(contact);
}

void ContactListFrontModel::removeContact(Contact *contact)
{
	static_cast<ContactListBaseModel*>(sourceModel())->onContactRemoved(contact);
}

void ContactListFrontModel::onServiceChanged(const QByteArray &name, QObject *newObject, QObject *oldObject)
{
	Q_UNUSED(newObject);
	Q_UNUSED(oldObject);
	if (name == m_comparator.name()) {
		connect(m_comparator, SIGNAL(contactChanged(qutim_sdk_0_3::Contact*)),
				sourceModel(), SLOT(onContactChanged(qutim_sdk_0_3::Contact*)));
		invalidate();
	} else if (name == m_model.name()) {
		ContactListBaseModel *oldModel = qobject_cast<ContactListBaseModel*>(oldObject);
		ContactListBaseModel *newModel = qobject_cast<ContactListBaseModel*>(newObject);
		if (newModel) {
			connect(newModel, SIGNAL(tagsChanged(QStringList)),
					this, SIGNAL(tagsChanged(QStringList)));
			connect(m_comparator, SIGNAL(contactChanged(qutim_sdk_0_3::Contact*)),
					newModel, SLOT(onContactChanged(qutim_sdk_0_3::Contact*)));
			foreach(Protocol *proto, Protocol::all()) {
				connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
						newModel, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
				foreach(Account *account, proto->accounts())
					newModel->onAccountCreated(account, !oldModel);
			}
			if (oldModel) {
				QSet<Contact*> contacts;
				oldModel->findContacts(contacts, oldModel->rootNode());
				foreach (Contact *contact, contacts)
					newModel->addContact(contact);
			}
		}
		setSourceModel(newModel);
	}
}

bool ContactListFrontModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
	const QRegExp regexp = filterRegExp();
	switch (index.data(ItemTypeRole).toInt()) {
	case ContactType: {
		Contact *contact = qobject_cast<Contact*>(index.data(BuddyRole).value<Buddy*>());
		Q_ASSERT(contact);
		if (!regexp.isEmpty()) {
			return contact->id().contains(regexp) || contact->name().contains(regexp);
		} else {
			if (!m_filterTags.isEmpty()) {
				bool hasAny = false;
				foreach (const QString &tag, contact->tags()) {
					hasAny |= bool(m_filterTags.contains(tag));
					if (hasAny)
						break;
				}
				if (!hasAny)
					return false;
			}
			if (!m_showOffline && !index.data(NotificationRole).toBool()) {
				const Status status = index.data(StatusRole).value<Status>();
				return status != Status::Offline;
			}
		}
		break;
	}
	case TagType: {
		if (!m_filterTags.isEmpty() && !m_filterTags.contains(index.data(TagNameRole).toString()))
			return false;
		int count = sourceModel()->rowCount(index);
		for (int i = 0; i < count; ++i) {
			if (filterAcceptsRow(i, index))
				return true;
		}
		return false;
	}
	default:
		break;
	};
	return true;
}

bool ContactListFrontModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	Contact *leftContact = qobject_cast<Contact*>(left.data(BuddyRole).value<Buddy*>());
	Contact *rightContact = qobject_cast<Contact*>(right.data(BuddyRole).value<Buddy*>());
	if (!leftContact || !rightContact) {
		// Show contacts always first
		return leftContact > rightContact;
	} else {
		return m_comparator->compare(leftContact, rightContact) < 0;
	}
}

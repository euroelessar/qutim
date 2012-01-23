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

#include "model.h"
#include <itemdelegate.h>
#include <qutim/metacontact.h>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/debug.h>
#include <avatarfilter.h>
#include "metacontactimpl.h"

namespace Core {
namespace MetaContacts {

using namespace qutim_sdk_0_3;

Model::Model(QObject *parent) :
    QStandardItemModel(parent),m_metaContact(0)
{
	m_metaRoot = new QStandardItem(QT_TRANSLATE_NOOP("MetaContacts", "Contacts in metacontact"));
	m_metaRoot->setData(true,SeparatorRole);
	appendRow(m_metaRoot);
	
	m_searchRoot = new QStandardItem(QT_TRANSLATE_NOOP("MetaContacts", "Search results"));
	m_searchRoot->setData(true,SeparatorRole);
	appendRow(m_searchRoot);
}

void Model::searchContacts(const QString& name)
{
	m_searchRoot->removeRows(0,m_searchRoot->rowCount());

	if(name.isEmpty())
		return;

	foreach(Account *account,Account::all()) {
		foreach(Contact *contact, account->findChildren<Contact*>()) {
			if(!contact->title().contains(name,Qt::CaseInsensitive))
				continue;
			if(m_metaContact && m_metaContact->contacts().contains(contact))
				continue;
			addContact(contact,m_searchRoot);
		}
	}

}

void Model::setMetaContact(MetaContactImpl *metaContact)
{
	m_metaContact = metaContact;
	//TODO remove children
	foreach(Contact *contact,metaContact->contacts())
		addContact(contact,m_metaRoot);
}

MetaContactImpl* Model::metaContact() const
{
	return m_metaContact;
}

void Model::addContact(Contact *contact , QStandardItem *root)
{
	for(int i=0;i!=root->rowCount();i++) {
		if(root->child(i)->data().value<Contact*>() == contact)
			return;
	}
	QStandardItem *item = new QStandardItem(contact->title());
	QIcon icon = AvatarFilter::icon(contact->avatar(),contact->status().icon());
	item->setIcon(icon);
	item->setData(qVariantFromValue(contact));
	QVariantMap map;
	map.insert(tr("Account"),contact->account()->id());
	item->setData(map,DescriptionRole);
	root->appendRow(item);
}

void Model::activated(const QModelIndex& index)
{
	//TODO optimize
	QStandardItem* item = itemFromIndex(index);
	Q_ASSERT(item);
	Contact *contact = item->data().value<Contact*>();
	if(!contact)
		return;
	if(item->parent() == m_metaRoot) {
		emit removeContactTriggered(contact);
	} else {
		addContact(contact,m_metaRoot);
		emit addContactTriggered(contact);
	}
	item->parent()->removeRow(index.row());
}

} // namespace MetaContacts
} // namespace Core


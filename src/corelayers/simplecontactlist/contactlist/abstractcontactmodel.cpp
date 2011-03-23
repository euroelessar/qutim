/****************************************************************************
 *  abstractcontactmodel.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "abstractcontactmodel.h"
#include <qutim/metacontactmanager.h>
#include <qutim/protocol.h>
#include <qutim/contact.h>

namespace Core {
namespace SimpleContactList {

using namespace qutim_sdk_0_3;

AbstractContactModel::AbstractContactModel(QObject *parent) :
    QAbstractItemModel(parent)
{
	connect(MetaContactManager::instance(), SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			this, SLOT(addContact(qutim_sdk_0_3::Contact*)));

	foreach(Protocol *proto, Protocol::all()) {
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach(Account *account, proto->accounts())
			onAccountCreated(account);
	}
}

AbstractContactModel::~AbstractContactModel()
{

}

bool AbstractContactModel::showOffline() const
{
	return true;
}

void AbstractContactModel::hideShowOffline()
{

}

void AbstractContactModel::filterList(const QString &)
{

}

void AbstractContactModel::filterList(const QStringList &)
{

}

QStringList AbstractContactModel::selectedTags() const
{
	return QStringList();
}

QStringList AbstractContactModel::tags() const
{
	return QStringList();
}

void AbstractContactModel::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	foreach (Contact *contact, account->findChildren<Contact *>()) {
		addContact(contact);
	}
	connect(account, SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			this, SLOT(addContact(qutim_sdk_0_3::Contact*)));
}


} // namespace SimpleContactList
} // namespace Core

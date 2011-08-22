/****************************************************************************
 *  vaccount.cpp
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

#include "vaccount.h"
#include <qutim/chatunit.h>
#include <qutim/passworddialog.h>
#include "vcontact.h"
#include "vkontakteprotocol.h"
#include "vconnection.h"
#include "vconnection_p.h"
#include "vroster.h"
#include "vaccount_p.h"
#include <qutim/inforequest.h>
#include "vinforequest.h"

VAccount::VAccount(const QString& email,QObject *parent) :
	Account(email, VkontakteProtocol::instance()),
	d_ptr(new VAccountPrivate)
{
	setParent(parent);
	setInfoRequestFactory(new VInfoFactory(this));
	Q_D(VAccount);
	d->q_ptr = this;
	setParent(protocol());
	d->connection = new VConnection(this,this);
	setStatus(Status::instance(Status::Offline,"vkontakte"));
}

VContact* VAccount::getContact(const QString& uid, bool create)
{
	return d_func()->connection->roster()->getContact(uid, create);
}

ChatUnit* VAccount::getUnit(const QString& unitId, bool create)
{
	return getContact(unitId,create);
}

void VAccount::loadSettings()
{
	d_func()->name = config().value("general/name", QString());
}

void VAccount::saveSettings()
{
	d_func()->connection->saveSettings();
}

VAccount::~VAccount()
{
//	saveSettings();
}

QString VAccount::name() const
{
	return d_func()->name;
}

void VAccount::setAccountName(const QString &name)
{
	Q_D(VAccount);
	if (d->name != name) {
		QString previous = d->name;
		d->name = name;
		config().setValue("general/name", name);
		emit nameChanged(name, previous);
	}
}

QString VAccount::password()
{
	Config cfg = config("general");
	QString password = cfg.value("passwd", QString(), Config::Crypted);
	if (password.isEmpty()) {
		PasswordDialog *dialog = PasswordDialog::request(this);
		if (dialog->exec() == PasswordDialog::Accepted) {
			password = dialog->password();
			if (dialog->remember())
				cfg.setValue("passwd", password, Config::Crypted);
		}
		dialog->deleteLater();
	}
	return password;
}

QString VAccount::uid() const
{
	return d_func()->uid;
}

void VAccount::setUid(const QString& uid)
{
	d_func()->uid = uid;
}

void VAccount::setStatus(Status status)
{
	Q_D(VAccount);
	VConnectionState state = statusToState(status.type());

	switch (state) {
		case Connected: {
			if (d->connection->connectionState() == Disconnected)
				d->connection->connectToHost(QString() /*password()*/);
			else if(d->connection->connectionState() == Connected)
				d->connection->roster()->setActivity(status);
			break;
		}
		case Disconnected: {
			if (d->connection->connectionState() != Disconnected)
				d->connection->disconnectFromHost();
				saveSettings();
			break;
		}
		default: {
			break;
		}		
	}
	Account::setStatus(status);
}

VConnection *VAccount::connection()
{
	return d_func()->connection;
}

const VConnection *VAccount::connection() const
{
	return d_func()->connection;
}

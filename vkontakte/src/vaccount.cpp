/****************************************************************************
 *  vaccount.cpp
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

VAccount::VAccount(const QString& email) : 
	Account(email, VkontakteProtocol::instance()),
	d_ptr(new VAccountPrivate)
{
	Q_D(VAccount);
	setParent(protocol());
	d->connection = new VConnection(this,this);
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
		d->name = name;
		emit nameChanged(name);
	}
}

QString VAccount::password()
{
	QString password = config().group("general").value("passwd", QString(), Config::Crypted);
	if (password.isEmpty()) {
		PasswordDialog *dialog = PasswordDialog::request(this);
		if (dialog->exec() == PasswordDialog::Accepted) {
			password = dialog->password();
			if (dialog->remember()) {
				config().group("general").setValue("passwd", password, Config::Crypted);
				config().sync();
			}
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
	Account::setStatus(status);
	Q_D(VAccount);
	status.initIcon("vkontakte");
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

	emit statusChanged(status);
}

VConnection *VAccount::connection()
{
	return d_func()->connection;
}

const VConnection *VAccount::connection() const
{
	return d_func()->connection;
}

bool VAccount::event(QEvent *ev)
{
	if (ev->type() == InfoRequestCheckSupportEvent::eventType()) {
		Status::Type status = this->status().type();
		if (status >= Status::Online && status <= Status::Invisible) {
			InfoRequestCheckSupportEvent *event = static_cast<InfoRequestCheckSupportEvent*>(ev);
			event->setSupportType(InfoRequestCheckSupportEvent::Read);
			event->accept();
		} else {
			ev->ignore();
		}
	} else if (ev->type() == InfoRequestEvent::eventType()) {
		InfoRequestEvent *event = static_cast<InfoRequestEvent*>(ev);
		event->setRequest(new VInfoRequest(this));
		event->accept();
	}
	return Account::event(ev);
}


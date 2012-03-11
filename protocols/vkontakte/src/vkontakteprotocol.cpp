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

#include "vkontakteprotocol.h"
#include "vkontakteprotocol_p.h"
#include <qutim/account.h>
#include "vaccount.h"
#include <qutim/statusactiongenerator.h>
#include "vcontact.h"
#include <QDesktopServices>
#include <QUrl>
#include <qutim/icon.h>
#include <QInputDialog>
#include <qutim/message.h>
#include <QDateTime>
#include <qutim/chatsession.h>
#include "vmessages.h"
#include "vconnection.h"
#include <qutim/settingslayer.h>
#include "ui/vaccountsettings.h"

VkontakteProtocol *VkontakteProtocol::self = 0;

VkontakteProtocol::VkontakteProtocol() :
		d_ptr(new VkontakteProtocolPrivate)
{
	Q_ASSERT(!self);
	self = this;
	d_func()->q_ptr = this;
}

VkontakteProtocol::~VkontakteProtocol()
{
	Settings::removeItem(m_mainSettings);
	delete m_mainSettings;
	m_mainSettings = 0;

	foreach (VAccount *acc, *d_func()->accounts)
		acc->saveSettings();
	self = 0;
}

Account* VkontakteProtocol::account(const QString& id) const
{
	Q_D(const VkontakteProtocol);
	return d->accounts_hash->value(id);
}
QList< Account* > VkontakteProtocol::accounts() const
{
	Q_D(const VkontakteProtocol);
	QList<Account *> accounts;
	QHash<QString, QPointer<VAccount> >::const_iterator it;
	for (it = d->accounts_hash->begin(); it != d->accounts_hash->end(); it++)
		accounts.append(it.value());
	return accounts;

}
void VkontakteProtocol::loadAccounts()
{
	Q_D(VkontakteProtocol);
	QList<Status> statuses;
	statuses << Status(Status::Online)
			<< Status(Status::Offline);
	foreach (Status status, statuses) {
		status.initIcon("vkontakte");
		Status::remember(status, "vkontakte");
		MenuController::addAction(new StatusActionGenerator(status), &VAccount::staticMetaObject);
	}

    ActionGenerator *gen = new ActionGenerator(Icon("applications-internet"),
							   QT_TRANSLATE_NOOP("Vkontakte","Open homepage"),
							   d,
							   SLOT(onOpenWebPageTriggered(QObject*)));
    gen->setType(ActionTypeContactList);
    MenuController::addAction<VContact>(gen);

//	static ActionGenerator sms_gen(Icon("phone"),
//							   QT_TRANSLATE_NOOP("Vkontakte","Send sms"),
//							   d,
//							   SLOT(onSendSmsTriggered(QObject*)));
//	sms_gen.setType(ActionTypeContactList);
//	MenuController::addAction<VContact>(&sms_gen);

	QStringList accounts = config("general").value("accounts", QStringList());
	foreach(const QString &uid, accounts) {
		VAccount *acc = new VAccount(uid,this);
		d->accounts_hash->insert(uid, acc);
		acc->loadSettings();
		connect(acc,SIGNAL(destroyed(QObject*)),d,SLOT(onAccountDestroyed(QObject*)));
		emit accountCreated(acc);
	}

	m_mainSettings = new GeneralSettingsItem<VAccountSettings>(Settings::Protocol,Icon("im-jabber"),QT_TRANSLATE_NOOP("Vkontakte","Account settings"));
	Settings::registerItem<VAccount>(m_mainSettings);
}

QVariant VkontakteProtocol::data(DataType type)
{
	switch (type) {
		case ProtocolIdName:
			return tr("id");
		case ProtocolContainsContacts:
			return true;
		default:
			return QVariant();
	}
}

void VkontakteProtocolPrivate::onAccountDestroyed(QObject *obj)
{
	VAccount *acc = reinterpret_cast<VAccount*>(obj);
	accounts->remove(accounts->key(acc));
}

void VkontakteProtocolPrivate::onOpenWebPageTriggered(QObject *obj)
{
	VContact *con = qobject_cast<VContact*>(obj);
	Q_ASSERT(obj);
	QUrl url ("http://vkontakte.ru/id" + con->id());
	QDesktopServices::openUrl(url);
}

bool VkontakteProtocol::event(QEvent *ev)
{
	return Protocol::event(ev);
}


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

#include "icqprotocol_p.h"
#include "icqaccount_p.h"
#include "icq_global.h"
#include "util.h"
#include "icqaccount.h"
#include "buddycaps.h"
#include <qutim/icon.h>
#include "ui/icqmainsettings.h"
#include "ui/icqaccountmainsettings.h"
#include <qutim/settingslayer.h>
#include <qutim/icon.h>
#include <qutim/systeminfo.h>
#include <QStringList>
#include <QWeakPointer>

namespace qutim_sdk_0_3 {

namespace oscar {

IcqProtocol *IcqProtocol::self = 0;

void IcqProtocolPrivate::removeAccount(QObject *obj)
{
	IcqAccount *const c = reinterpret_cast<IcqAccount*>(obj);
	accounts->remove(accounts->key(c));
}

IcqProtocol::IcqProtocol() :
	d_ptr(new IcqProtocolPrivate)
{
	Q_ASSERT(!self);
	self = this;
}

IcqProtocol::~IcqProtocol()
{
	self = 0;
}

void IcqProtocol::loadAccounts()
{
	Q_D(IcqProtocol);

	Settings::registerItem(new GeneralDataSettingsItem<IcqMainSettings>(
							   Settings::Protocol,
							   Icon("im-icq"),
							   QT_TRANSLATE_NOOP_UTF8("Settings", "Icq")));
	Settings::registerItem<IcqAccount>(
				new GeneralSettingsItem<IcqAccountMainSettingsWidget>(
					Settings::Protocol,
					Icon("im-icq"),
					QT_TRANSLATE_NOOP_UTF8("Settings", "Icq account settings")));
	updateSettings();

	Q_UNUSED(OscarStatus());
	QStringList accounts = config("general").value("accounts", QStringList());
	foreach(const QString &uin, accounts) {
		IcqAccount *acc = new IcqAccount(uin);
		d->accounts_hash->insert(uin, acc);
		acc->updateSettings();
		emit accountCreated(acc);
		acc->d_func()->loadRoster();
	}
}

void IcqProtocol::virtual_hook(int id, void *data)
{
	switch (id) {
	case SupportedAccountParametersHook: {
		QStringList &properties = *reinterpret_cast<QStringList*>(data);
		properties << QLatin1String("password");
		break;
	}
	case CreateAccountHook: {
		CreateAccountArgument &argument = *reinterpret_cast<CreateAccountArgument*>(data);
		argument.account = new IcqAccount(argument.id);
		QString password = argument.parameters.value(QLatin1String("password")).toString();
		Config cfg = argument.account->config();
		cfg.beginGroup("general");
		if (!password.isEmpty())
			cfg.setValue("passwd", password, Config::Crypted);
		else
			cfg.remove("passwd");
		addAccount(static_cast<IcqAccount*>(argument.account));
		break;
	}
	default:
		Protocol::virtual_hook(id, data);
	}
}

QList<Account *> IcqProtocol::accounts() const
{
	Q_D(const IcqProtocol);
	QList<Account *> accounts;
	QHash<QString, QWeakPointer<IcqAccount> >::const_iterator it;
	for (it = d->accounts_hash->begin(); it != d->accounts_hash->end(); it++)
		accounts.append(it.value().data());
	return accounts;
}

Account *IcqProtocol::account(const QString &id) const
{
	Q_D(const IcqProtocol);
	return d->accounts_hash->value(id).data();
}

QHash<QString, IcqAccount *> IcqProtocol::accountsHash() const
{
	return *d_func()->accounts;
}

void IcqProtocol::addAccount(IcqAccount *account)
{
	Q_D(IcqProtocol);
	Config cfg = config("general");
	QStringList accounts = cfg.value("accounts", QStringList());
	accounts << account->id();
	cfg.setValue("accounts", accounts);
	account->updateSettings();
	d->accounts_hash->insert(account->id(), account);
	emit accountCreated(account);
	account->d_func()->loadRoster();
	connect(account,SIGNAL(destroyed(QObject*)),d,SLOT(removeAccount(QObject*)));
}

void IcqProtocol::updateSettings()
{
	Q_D(IcqProtocol);
	Config cfg = config("general");
	QString localeCodecName = QLatin1String(QTextCodec::codecForLocale()->name());
	QString codecName = cfg.value("codec", localeCodecName);
	QTextCodec *codec = QTextCodec::codecForName(codecName.toLatin1());
	Util::setAsciiCodec(codec ? codec : QTextCodec::codecForLocale());
	foreach (QWeakPointer<IcqAccount> acc, *d->accounts_hash)
		acc.data()->updateSettings();
	emit settingsUpdated();
}

QVariant IcqProtocol::data(DataType type)
{
	switch (type) {
	case ProtocolIdName:
		return "UIN";
	case ProtocolContainsContacts:
		return true;
	default:
		return QVariant();
	}
}

} } // namespace qutim_sdk_0_3::oscar


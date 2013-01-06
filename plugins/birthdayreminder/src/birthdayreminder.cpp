/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "birthdayreminder.h"
#include "birthdayremindersettings.h"
#include <qutim/debug.h>
#include <qutim/notification.h>
#include <qutim/utils.h>
#include <qutim/settingslayer.h>
#include <qutim/icon.h>

static bool isStatusOnline(const Status &status)
{
	Status::Type type = status.type();
	return type != Status::Offline && type != Status::Connecting;
}

BirthdayUpdater::BirthdayUpdater(Account *account, InfoRequestFactory *factory, QObject *parent) :
	QObject(parent),
	m_account(account),
	m_factory(factory),
	m_updateFails(0)
{
	m_factory = account->infoRequestFactory();
	m_updateTimer.setInterval(30000);
	connect(&m_updateTimer, SIGNAL(timeout()), SLOT(onUpdateNext()));
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(onStatusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
}

void BirthdayUpdater::update(Contact *contact)
{
	Q_ASSERT(contact->account() == m_account);
	if (m_waitingUpdate.contains(contact))
		return;
	if (m_waitingUpdate.isEmpty() && isStatusOnline(m_account->status()))
		m_updateTimer.start();
	m_waitingUpdate.push_back(contact);
}

void BirthdayUpdater::onUpdateNext()
{
	Contact *contact = 0;
	QMutableListIterator<QWeakPointer<Contact> > itr(m_waitingUpdate);
	while (itr.hasNext()) {
		if (!itr.next()) {
			itr.remove();
			continue;
		}
		contact = itr.value().data();
		InfoRequestFactory::SupportLevel level = m_factory->supportLevel(contact);
		if (level >= InfoRequestFactory::ReadOnly) {
			m_updateFails = 0;
			itr.remove();
			break;
		} else {
			contact = NULL;
		}
	}
	
	if (!contact) {
		if (++m_updateFails >= 30) {
			// We have been trying to update the contacts for a long time now.
			// Maybe their protocol is not supported information requests
			// despite its saying otherwise. So just forget about the contacts.
			m_waitingUpdate.clear();
			m_updateFails = 0;
			m_updateTimer.stop();
		}
		return;
	}
	
	static QSet<QString> hints = QSet<QString>() << "birthday";
	InfoRequest *request = m_factory->createrDataFormRequest(contact);
	connect(request, SIGNAL(stateChanged(qutim_sdk_0_3::InfoRequest::State)),
			SLOT(onRequestStateChanged(qutim_sdk_0_3::InfoRequest::State)));
	request->requestData(hints);
	if (m_waitingUpdate.isEmpty())
		m_updateTimer.stop();
}

void BirthdayUpdater::onRequestStateChanged(qutim_sdk_0_3::InfoRequest::State state)
{
	if (state == InfoRequest::RequestDone) {
		InfoRequest *request = sender_cast<InfoRequest*>(sender());
		request->deleteLater();
		Contact *contact = qobject_cast<Contact*>(request->object());
		if (!contact)
			return;
		emit birthdayUpdated(contact, request->value("birthday", QDate()));
	} else if (state == InfoRequest::Canceled || state == InfoRequest::Error) {
		sender()->deleteLater();
	}
}

void BirthdayUpdater::onStatusChanged(const qutim_sdk_0_3::Status &current,
									  const qutim_sdk_0_3::Status &previous)
{
	bool isOnline = isStatusOnline(current);
	bool wasOnline = isStatusOnline(previous);
	if (isOnline && !wasOnline) {
		if (!m_waitingUpdate.isEmpty())
			m_updateTimer.start();
	} else if (!isOnline && wasOnline) {
		m_updateTimer.stop();
	}
}

BirthdayReminder::BirthdayReminder() :
	m_daysBeforeNotification(0)
{
}

void BirthdayReminder::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "BirthdayReminder"),
			QT_TRANSLATE_NOOP("Plugin", "This plugin shows notifications when someone from "
							  "your contact list has a birthday"),
			PLUGIN_VERSION(0, 1, 0, 0));
	setCapabilities(Loadable);
}

bool BirthdayReminder::load()
{
	reloadSettings();
	foreach (Protocol *proto, Protocol::all()) {
		foreach (Account *account, proto->accounts())
			onAccountCreated(account);
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
	}
	connect(&m_notificationTimer, SIGNAL(timeout()), SLOT(onNotificationTimeout()));
	m_notificationTimer.start();
	
	m_settings = new GeneralSettingsItem<BirthdayReminderSettings>(
					 Settings::Plugin,
					 Icon(QLatin1String("view-calendar-birthday")),
					 QT_TRANSLATE_NOOP("Settings", "Birthday reminder"));
	m_settings->connect(SIGNAL(saved()), this, SLOT(reloadSettings()));
	Settings::registerItem(m_settings);
	
	return true;
}

bool BirthdayReminder::unload()
{
	m_notificationTimer.stop();
	foreach (Protocol *proto, Protocol::all())
		disconnect(proto, 0, this, 0);
	
	QHashIterator<Account*, BirthdayUpdater*> itr(m_accounts);
	while (itr.hasNext()) {
		itr.next();
		itr.value()->deleteLater();
		disconnect(itr.key(), 0, this, 0);
	}
	m_accounts.clear();
	
	Settings::removeItem(m_settings);
	delete m_settings; m_settings = 0;
	
	return true;
}

void BirthdayReminder::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	InfoRequestFactory *factory = account->infoRequestFactory();
	if (!factory)
		return;
	
	BirthdayUpdater *updater = new BirthdayUpdater(account, factory, this);
	m_accounts.insert(account, updater);
	connect(updater, SIGNAL(birthdayUpdated(Contact*,QDate)),
			SLOT(onBirthdayUpdated(Contact*,QDate)));
	connect(account, SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			SLOT(onContactCreated(qutim_sdk_0_3::Contact*)));
	connect(account, SIGNAL(destroyed(QObject*)),
			SLOT(onAccountDestroyed(QObject*)));
	
	checkAccount(account, updater, factory);
}

void BirthdayReminder::onAccountDestroyed(QObject *obj)
{
	BirthdayUpdater *updater = m_accounts.take(static_cast<Account*>(obj));
	if (updater)
		updater->deleteLater();
}

void BirthdayReminder::onContactCreated(qutim_sdk_0_3::Contact *contact)
{
	QDate currentDate = QDate::currentDate();
	Account *acc = contact->account();
	Config cfg = acc->config(QLatin1String("storedBirthdays"));
	BirthdayUpdater *updater = m_accounts.value(acc);
	Q_ASSERT(updater);
	checkContact(contact, updater, acc->infoRequestFactory(), cfg, currentDate);
}

void BirthdayReminder::onBirthdayUpdated(Contact *contact, const QDate &birthday)
{
	QDate current = QDate::currentDate();
	checkContactBirthday(contact, birthday, current);
	
	Config cfg = contact->account()->config(QLatin1String("storedBirthdays"));
	cfg.beginGroup(contact->id());
	cfg.setValue(QLatin1String("birthday"), birthday);
	cfg.setValue(QLatin1String("lastUpdateDate"), current);
	cfg.endGroup();
}

void BirthdayReminder::onNotificationTimeout()
{
	QHashIterator<Account*, BirthdayUpdater*> itr(m_accounts);
	while (itr.hasNext()) {
		itr.next();
		checkAccount(itr.key(), itr.value(), itr.key()->infoRequestFactory());
	}
}

void BirthdayReminder::reloadSettings()
{
	Config cfg;
	cfg.beginGroup("birthdayReminder");
	m_daysBeforeNotification = cfg.value("daysBeforeNotification", 3);
	int notifInterval = cfg.value("intervalBetweenNotifications", 24.0) * 60 * 60 * 1000;
	if (notifInterval != m_notificationTimer.interval())
		m_notificationTimer.setInterval(notifInterval);
	cfg.endGroup();
}

void BirthdayReminder::checkContactBirthday(Contact *contact, const QDate &birthday, const QDate &current)
{
	if (!birthday.isValid())
		return;

	QDate nextBirthday = birthday.addYears(current.year() - birthday.year());
	if (nextBirthday < current)
		nextBirthday = nextBirthday.addYears(1);
	int daysToBirthday = current.daysTo(nextBirthday);
	
	if (daysToBirthday < m_daysBeforeNotification) {
		QString contactTitle = contact->title();
		QString text;
		switch (daysToBirthday) {
		case 0:
			text = tr("It's %1's birthday today! Hooray!");
			break;
		case 1:
			text = tr("%1's birthday will be tomorrow! Hooray!");
			break;
		default:
			text = tr("%2's birthday is in %1 days", 0, daysToBirthday).arg(daysToBirthday);
			break;
		}
		text = text.arg(contactTitle);
		
		NotificationRequest request(Notification::UserHasBirthday);
		request.setTitle(tr("%1's birhday").arg(contactTitle));
		request.setText(text);
		request.setObject(contact);
		request.setProperty("birthday", birthday);
		request.setProperty("nextBirthday", nextBirthday);
		request.send();
	}
}

void BirthdayReminder::checkAccount(Account *account, BirthdayUpdater *updater, InfoRequestFactory *factory)
{
	Config cfg = account->config(QLatin1String("storedBirthdays"));
	foreach (Contact *contact, account->findChildren<Contact*>())
		checkContact(contact, updater, factory, cfg, QDate::currentDate());
}

void BirthdayReminder::checkContact(Contact *contact, BirthdayUpdater *updater,
									InfoRequestFactory *factory, Config &cfg,
									const QDate &currentDate)
{
	if (factory->supportLevel(contact) == InfoRequestFactory::NotSupported)
		return;
	
	cfg.beginGroup(contact->id());
	checkContactBirthday(contact, cfg.value(QLatin1String("birthday"), QDate()), currentDate);
	QDate lastUpdate = cfg.value(QLatin1String("lastUpdateDate"), QDate());
	int daysSinceLastUpdate = lastUpdate.daysTo(currentDate);
	if (daysSinceLastUpdate < 0 || daysSinceLastUpdate > 3)
		updater->update(contact);
	cfg.endGroup();
}

QUTIM_EXPORT_PLUGIN(BirthdayReminder)


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
#include "icqsupport.h"
#include <qutim/event.h>
#include <qutim/account.h>
#include <QApplication>
#include "nowplaying.h"
#include "ui_icqsettings.h"
#include <QTime>

namespace qutim_sdk_0_3 {

namespace nowplaying {

	IcqTuneStatus::IcqTuneStatus()
	{
	}

	IcqTuneStatus::IcqTuneStatus(Account *account, IcqTuneStatus *factory) :
			AccountTuneStatus(account, factory), m_icqFactory(factory)
	{
		icqChangeXstatusEvent = Event::registerType("icq-change-xstatus");
		icqXstatusAboutToBeChanged = Event::registerType("icq-xstatus-about-to-be-changed");
		m_account->installEventFilter(this);
	}

	void IcqTuneStatus::setStatus(const TrackInfo &info)
	{
		OscarSettings config = NowPlaying::instance()->forAllAccounts() ?
							   m_icqFactory->m_settings :
							   m_settings;
		if (config.deactivated)
			return;
		currentMessage = message(info);
		QVariantHash xstatus = m_account->property("xstatus").toHash();
		QString xstatusName = xstatus.value("name").toString();
		if (config.setsCurrentStatus && xstatus.value("description").toString() == currentMessage) {
			return;
		} else if (config.setsMusicStatus &&
				  (xstatusName != "listening_to_music" ||
				   xstatus.value("description").toString() == currentMessage))
		{
			return;
		}

		if (xstatusName.isEmpty())
			xstatus.insert("name", "listening_to_music");
		xstatus.insert("description", currentMessage);
		Event ev(icqChangeXstatusEvent);
		ev.args[0] = xstatus;
		qApp->sendEvent(m_account, &ev);
	}

	void IcqTuneStatus::removeStatus()
	{
		currentMessage = QString();
		Event ev(icqChangeXstatusEvent);
		qApp->sendEvent(m_account, &ev);
	}

	void IcqTuneStatus::loadSettings()
	{
		Config cfg = config(m_account ? m_account->id() : "oscar");
		m_settings.deactivated = cfg.value("deactivated", false);
		m_settings.setsCurrentStatus = cfg.value("setCurrentStatus", false);
		m_settings.setsMusicStatus = cfg.value("setMusicStatus", true);
		m_settings.mask_1 = cfg.value("mask1", QString("Now playing: %artist - %title"));
		m_settings.mask_2 = cfg.value("mask2", QString("%artist - %title"));
	}

	AccountTuneSettings *IcqTuneStatus::settingsWidget()
	{
		return new IcqTuneSettings();
	}

	AccountTuneStatus *IcqTuneStatus::construct(Account *account, AccountTuneStatus *factory)
	{
		return new IcqTuneStatus(account, qobject_cast<IcqTuneStatus*>(factory));
	}

	bool IcqTuneStatus::eventFilter(QObject *obj, QEvent *e)
	{
		if (obj == m_account && e->type() == qutim_sdk_0_3::Event::eventType()) {
			qutim_sdk_0_3::Event *customEvent = static_cast<qutim_sdk_0_3::Event*>(e);
			if (customEvent->id == icqXstatusAboutToBeChanged) {
				if (!NowPlaying::instance()->isWorking() && m_settings.deactivated)
					return false;
				QVariantHash xstatus = customEvent->at<QVariantHash>(0);
				if (m_settings.setsCurrentStatus ||
					(m_settings.setsMusicStatus && xstatus.value("name").toString() == "listening_to_music"))
				{
					xstatus.insert("description", currentMessage);
					customEvent->args[0] = xstatus;
				} else if (m_settings.setsMusicStatus && !currentMessage.isEmpty() &&
						   xstatus.value("description").toString() == currentMessage)
				{
					xstatus.insert("description", QString());
					customEvent->args[0] = xstatus;
				}
				return false;
			}
		}
		return AccountTuneStatus::eventFilter(obj, e);
	}

	QString IcqTuneStatus::message(const TrackInfo &info)
	{
		OscarSettings config = NowPlaying::instance()->forAllAccounts() ?
							   m_settings :
							   m_icqFactory->m_settings;
		QString message = (config.setsCurrentStatus) ? config.mask_1 : config.mask_2;
		QTime time(0, 0, 0);
		time.addSecs(info.time);
		message.replace("%artist", info.artist)
				.replace("%title", info.title)
				.replace("%album", info.album)
				.replace("%track", QString::number(info.trackNumber))
				.replace("%file", info.location.toString(QUrl::RemoveScheme | QUrl::RemoveAuthority))
				.replace("%uri", info.location.toString())
				.replace("%time", time.toString("H:mm:ss"));
		return message;
	}

	IcqTuneSettings::IcqTuneSettings() :
		ui(new Ui::IcqSettings)
	{
		ui->setupUi(this);
	}

	IcqTuneSettings::~IcqTuneSettings()
	{
		delete ui;
	}

	void IcqTuneSettings::loadState(AccountTuneStatus *acc)
	{
		IcqTuneStatus *account = qobject_cast<IcqTuneStatus*>(acc);
		if (!account)
			return;
		OscarSettings settings = m_settings.contains(account) ?
						 m_settings.value(account) :
						 account->settings();
		ui->oscar_deactivated->setChecked(settings.deactivated);
		ui->oscar_change_current->setChecked(settings.setsCurrentStatus);
		ui->oscar_change_music_status->setChecked(settings.setsMusicStatus);
		ui->oscar_mask_1->setText(settings.mask_1);
		ui->oscar_mask_2->setText(settings.mask_2);
	}

	void IcqTuneSettings::saveState(AccountTuneStatus *acc)
	{
		IcqTuneStatus *account = qobject_cast<IcqTuneStatus*>(acc);
		if (!account)
			return;
		OscarSettings settings;
		settings.deactivated = ui->oscar_deactivated->isChecked();
		settings.setsCurrentStatus = ui->oscar_change_current->isChecked();
		settings.setsMusicStatus = ui->oscar_change_music_status->isChecked();
		settings.mask_1 = ui->oscar_mask_1->text();
		settings.mask_2 = ui->oscar_mask_2->text();
		m_settings.insert(account, settings);
	}

	void IcqTuneSettings::clearStates()
	{
		m_settings.clear();
	}

	void IcqTuneSettings::saveConfigs()
	{
		QHashIterator<IcqTuneStatus*, OscarSettings> itr(m_settings);
		while (itr.hasNext()) {
			itr.next();
			Account *acc = itr.key()->account();
			const OscarSettings &settings = itr.value();
			Config cfg = config(acc ? acc->id() : "oscar");
			cfg.setValue("deactivated", settings.deactivated);
			cfg.setValue("setCurrentStatus", settings.setsCurrentStatus);
			cfg.setValue("setMusicStatus", settings.setsMusicStatus);
			cfg.setValue("mask1", settings.mask_1);
			cfg.setValue("mask2", settings.mask_2);
		}

	}

} }


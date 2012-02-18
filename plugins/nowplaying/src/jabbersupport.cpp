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
#include "jabbersupport.h"
#include <qutim/event.h>
#include <qutim/account.h>
#include <QApplication>
#include "ui_jabbersettings.h"
#include "nowplaying.h"

namespace qutim_sdk_0_3 {

namespace nowplaying {

	JabberTuneStatus::JabberTuneStatus()
	{
	}

	JabberTuneStatus::JabberTuneStatus(Account *account, JabberTuneStatus *factory) :
		AccountTuneStatus(account, factory), m_jabberFactory(factory)
	{
		jabberPersonalEvent = Event::registerType("jabber-personal-event");
	}

	void JabberTuneStatus::setStatus(const TrackInfo &info)
	{
		JabberSettings config = NowPlaying::instance()->forAllAccounts() ?
								m_jabberFactory->m_settings :
								m_settings;
		if (config.deactivated)
			return;
		QVariantHash tune;
		if (config.album)
			tune.insert("source", info.album);
		if (config.artist)
			tune.insert("artist", info.artist);
		if (config.length)
			tune.insert("length", info.time);
		if (config.number)
			tune.insert("track", QString::number(info.trackNumber));
		if (config.title)
			tune.insert("title", info.title);
		if (config.uri)
			tune.insert("uri", info.location.toString());
		Event ev(jabberPersonalEvent, "tune", tune, true);
		qApp->sendEvent(m_account, &ev);
	}

	void JabberTuneStatus::removeStatus()
	{
		Event ev(jabberPersonalEvent, "tune", QVariantHash(), true);
		qApp->sendEvent(m_account, &ev);
	}

	void JabberTuneStatus::loadSettings()
	{
		Config cfg = config(m_account ? m_account->id() : "jabber");
		m_settings.deactivated = cfg.value("deactivated", false);
		m_settings.artist = cfg.value("artist", true);
		m_settings.title = cfg.value("title", true);
		m_settings.album = cfg.value("album", false);
		m_settings.length = cfg.value("length", false);
		m_settings.number = cfg.value("number", false);
		m_settings.uri = cfg.value("uri", false);
	}

	AccountTuneSettings *JabberTuneStatus::settingsWidget()
	{
		return new JabberTuneSettings();
	}

	AccountTuneStatus *JabberTuneStatus::construct(Account *account, AccountTuneStatus *factory)
	{
		return new JabberTuneStatus(account, qobject_cast<JabberTuneStatus*>(factory));
	}

	JabberTuneSettings::JabberTuneSettings() :
		ui(new Ui::JabberSettings)
	{
		ui->setupUi(this);
	}

	JabberTuneSettings::~JabberTuneSettings()
	{
		delete ui;
	}

	void JabberTuneSettings::loadState(AccountTuneStatus *acc)
	{
		JabberTuneStatus *account = qobject_cast<JabberTuneStatus*>(acc);
		if (!account)
			return;
		JabberSettings settings = m_settings.contains(account) ?
								  m_settings.value(account) :
								  account->settings();
		bool active = !settings.deactivated;
		ui->jabber_deactivated->setChecked(settings.deactivated);
		ui->jabber_activated->setChecked(active);
		ui->jabber_artist->setChecked(settings.artist);
		ui->jabber_title->setChecked(settings.title);
		ui->jabber_album->setChecked(settings.album);
		ui->jabber_track_number->setChecked(settings.number);
		ui->jabber_length->setChecked(settings.length);
		ui->jabber_uri->setChecked(settings.uri);
	}

	void JabberTuneSettings::saveState(AccountTuneStatus *acc)
	{
		JabberTuneStatus *account = qobject_cast<JabberTuneStatus*>(acc);
		if (!account)
			return;
		JabberSettings settings = account->settings();
		settings.deactivated = ui->jabber_deactivated->isChecked();
		settings.artist = ui->jabber_artist->isChecked();
		settings.title = ui->jabber_title->isChecked();
		settings.album = ui->jabber_album->isChecked();
		settings.number = ui->jabber_track_number->isChecked();
		settings.length = ui->jabber_length->isChecked();
		settings.uri = ui->jabber_uri->isChecked();
		m_settings.insert(account, settings);
	}

	void JabberTuneSettings::clearStates()
	{
		m_settings.clear();
	}

	void JabberTuneSettings::saveConfigs()
	{
		QHashIterator<JabberTuneStatus*, JabberSettings> itr(m_settings);
		while (itr.hasNext()) {
			itr.next();
			Account *acc = itr.key()->account();
			const JabberSettings &settings = itr.value();
			Config cfg = config(acc ? acc->id() : "jabber");
			cfg.setValue("deactivated", settings.deactivated);
			cfg.setValue("artist", settings.artist);
			cfg.setValue("title", settings.title);
			cfg.setValue("album", settings.album);
			cfg.setValue("length", settings.length);
			cfg.setValue("number", settings.number);
			cfg.setValue("uri", settings.uri);
		}
		clearStates();
	}

} }


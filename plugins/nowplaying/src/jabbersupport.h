/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#ifndef JABBERSUPPORT_H
#define JABBERSUPPORT_H

#include "accounttunestatus.h"
#include <QHash>

namespace Ui {
	class JabberSettings;
}

namespace qutim_sdk_0_3 {

namespace nowplaying {

	struct JabberSettings
	{
		bool deactivated;
		bool artist;
		bool title;
		bool album;
		bool number;
		bool length;
		bool uri;
	};

	class JabberTuneStatus : public AccountTuneStatus
	{
		Q_OBJECT
	public:
		JabberTuneStatus();
		JabberTuneStatus(Account *account, JabberTuneStatus *factory);
		virtual void setStatus(const TrackInfo &info);
		virtual void removeStatus();
		virtual void loadSettings();
		virtual AccountTuneSettings *settingsWidget();
		virtual AccountTuneStatus *construct(Account *account, AccountTuneStatus *factory);
		JabberSettings settings() { return m_settings; }
	private:
		JabberSettings m_settings;
		JabberTuneStatus *m_jabberFactory;
		quint16 jabberPersonalEvent;
	};

	class JabberTuneSettings : public AccountTuneSettings
	{
		Q_OBJECT
	public:
		JabberTuneSettings();
		~JabberTuneSettings();
	protected:
		virtual void loadState(AccountTuneStatus *account);
		virtual void saveState(AccountTuneStatus *account);
		virtual void clearStates();
		virtual void saveConfigs();
	private:
		Ui::JabberSettings *ui;
		QHash<JabberTuneStatus*, JabberSettings> m_settings;
	};

} }

#endif // JABBERSUPPORT_H


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
#ifndef ICQSUPPORT_H
#define ICQSUPPORT_H

#include "accounttunestatus.h"
#include <QHash>

namespace Ui {
	class IcqSettings;
}

namespace qutim_sdk_0_3 {

namespace nowplaying {

	struct OscarSettings
	{
		bool deactivated;
		bool setsCurrentStatus;
		bool setsMusicStatus;
		QString mask_1;
		QString mask_2;
	};

	class IcqTuneStatus : public AccountTuneStatus
	{
		Q_OBJECT
	public:
		IcqTuneStatus();
		IcqTuneStatus(Account *account, IcqTuneStatus *factory);
		virtual void setStatus(const TrackInfo &info);
		virtual void removeStatus();
		virtual void loadSettings();
		virtual AccountTuneSettings *settingsWidget();
		virtual AccountTuneStatus *construct(Account *account, AccountTuneStatus *factory);
		OscarSettings settings() { return m_settings; }
	protected:
		bool eventFilter(QObject *obj, QEvent *e);
	private:
		QString message(const TrackInfo &info);
		OscarSettings m_settings;
		IcqTuneStatus *m_icqFactory;
		quint16 icqChangeXstatusEvent;
		quint16 icqXstatusAboutToBeChanged;
		QString currentMessage;
	};

	class IcqTuneSettings : public AccountTuneSettings
	{
	public:
		IcqTuneSettings();
		~IcqTuneSettings();
	protected:
		virtual void loadState(AccountTuneStatus *account);
		virtual void saveState(AccountTuneStatus *account);
		virtual void clearStates();
		virtual void saveConfigs();
	private:
		Ui::IcqSettings *ui;
		QHash<IcqTuneStatus*, OscarSettings> m_settings;
	};

} }

#endif // ICQSUPPORT_H


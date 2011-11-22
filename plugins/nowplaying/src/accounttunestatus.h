/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef ACCOUNTTUNESTATUS_H
#define ACCOUNTTUNESTATUS_H

#include "../include/trackinfo.h"
#include <QWidget>

namespace qutim_sdk_0_3 {

class Account;
class SettingsWidget;

namespace nowplaying {

	class AccountTuneSettings;

	class AccountTuneStatus : public QObject
	{
		Q_OBJECT
	public:
		AccountTuneStatus(Account *account = 0, AccountTuneStatus *factory = 0);
		virtual void setStatus(const TrackInfo &info) = 0;
		virtual void removeStatus() = 0;
		virtual void loadSettings() = 0;
		virtual AccountTuneSettings *settingsWidget() { return 0; }
		virtual AccountTuneStatus *construct(Account *account, AccountTuneStatus *factory) = 0;
		Account *account() { return m_account; }
		AccountTuneStatus *factory() { return m_factory; }
	protected:
		Account *m_account;
		AccountTuneStatus *m_factory;
	};

	class AccountTuneSettings : public QWidget
	{
	public:
		virtual void loadState(AccountTuneStatus *account) = 0;
		virtual void saveState(AccountTuneStatus *account) = 0;
		virtual void clearStates() = 0;
		virtual void saveConfigs() = 0;
	};

} }

#endif // ACCOUNTTUNESTATUS_H


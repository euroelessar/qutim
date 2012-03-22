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
#ifndef SETTINGSUI_H
#define SETTINGSUI_H

#include "nowplaying.h"
#include <qutim/settingswidget.h>
#include <qutim/configbase.h>
#include <qutim/libqutim_global.h>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <QHash>
#include <QDebug>
#include <QWeakPointer>
#include <qutim/dataforms.h>

namespace Ui {
	class NowPlayingSettings;
}

namespace qutim_sdk_0_3 {

namespace nowplaying {

	class AccountTuneSettings;
	class AccountTuneStatus;
	class NowPlaying;

	class SettingsUI : public SettingsWidget
	{
		Q_OBJECT
	public:
		SettingsUI();
		~SettingsUI();
		virtual void loadImpl();
		virtual void saveImpl();
		virtual void cancelImpl();
	signals:
		void configSaved();
		void statusChanged(bool);
	public slots:
		void on_accounts_currentIndexChanged(int index);
		void on_changeStatus_clicked();
		void on_forAllAccounts_clicked();
		void on_playerSettings_currentChanged(int index);
	private:
		void updateFields();
		void updateStatusText();
		void saveState();
		Ui::NowPlayingSettings *ui;
		bool m_enableForAllAccounts;
		QList<AccountTuneStatus*> m_accounts;
		QWeakPointer<AccountTuneStatus> m_currentAccount;
		QHash<AccountTuneStatus*, AccountTuneSettings*> m_settingWidgets;
		QHash<QString, QWidget*> m_playerWidgets;
		NowPlaying *m_manager;
	};

} }
#endif


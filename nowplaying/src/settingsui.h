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
#include <QPointer>

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
		void accountChanged(int index);
		void stopButtonClicked();
		void forAllAccountsClicked();
	private:
		void updateFields();
		void updateStatusText();
		void saveState();
		Ui::NowPlayingSettings *ui;
		bool m_enableForAllAccounts;
		QList<AccountTuneStatus*> m_accounts;
		QPointer<AccountTuneStatus> m_currentAccount;
		QHash<AccountTuneStatus*, AccountTuneSettings*> m_settingWidgets;
		NowPlaying *m_manager;
	};

} }
#endif

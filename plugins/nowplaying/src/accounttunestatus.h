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

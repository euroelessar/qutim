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
	private:
		QString message(const TrackInfo &info);
		OscarSettings m_settings;
		IcqTuneStatus *m_icqFactory;
		quint16 icqChangeXstatusEvent;
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

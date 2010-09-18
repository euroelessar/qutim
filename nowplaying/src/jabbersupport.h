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

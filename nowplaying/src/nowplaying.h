#ifndef NOWPLAYING_H
#define NOWPLAYING_H

#include "settingsui.h"
#include "player.h"
#include <qutim/plugin.h>
#include <qutim/actiongenerator.h>
#include <qutim/config.h>
#include <QHash>

namespace qutim_sdk_0_3 {

class Account;
class Protocol;

namespace nowplaying {

	class AccountTuneStatus;
	class StopStartActionGenerator;

	class NowPlaying : public Plugin
	{
		Q_OBJECT
		Q_CLASSINFO("DebugInfo", "NowPlaying")
		Q_CLASSINFO("Service", "NowPlaying")
	public:
		NowPlaying();
		void init();
		bool load();
		bool unload();
		QHash<Protocol*, AccountTuneStatus*> factories() { return m_factories; }
		QList<AccountTuneStatus*> accounts() { return m_accounts; }
		QHash<QString, Player*> players() { return m_players; }
		Player *currentPlayer() { return m_player; }
		bool forAllAccounts() { return m_forAllAccounts; }
		bool isWorking() { return m_isWorking; }
		static NowPlaying *instance() { Q_ASSERT(self); return self; }
	public slots:
		void loadSettings();
		void setState(bool isWorking);
	private slots:
		void stopStartActionTrigged();
		void playingStatusChanged(bool);
		void accountCreated(qutim_sdk_0_3::Account*);
		void accountDeleted();
		void setStatuses(const TrackInfo &info);
	private:
		void initPlayer(const QString &playerName);
		void clearStatuses();
	private:
		StopStartActionGenerator* m_stopStartAction;
		QHash<QString, Player*> m_players;
		Player* m_player;
		QHash<Protocol*, AccountTuneStatus*> m_factories;
		QList<AccountTuneStatus*> m_accounts;
		bool m_isWorking;
		bool m_forAllAccounts;
		static NowPlaying *self;
	};

	class StopStartActionGenerator : public ActionGenerator
	{
	public:
		StopStartActionGenerator(QObject *module, bool isWorking);
		void showImpl(QAction *action, QObject *obj);
		void setState(bool isWorking);
	private:
		QString m_text;
	};

	inline qutim_sdk_0_3::Config config(const QString &group = QString())
	{
		qutim_sdk_0_3::Config cfg("nowplaying");
		return !group.isEmpty() ? cfg.group(group) : cfg;
	}
} }
#endif // NOWPLAYING_H

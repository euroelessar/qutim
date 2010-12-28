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

template<typename T>
class HookPointer
{
public:
	HookPointer() : m_obj(0), m_ptr(0) {}
	HookPointer(QObject *obj) : m_obj(obj), m_ptr(qobject_cast<T*>(obj)) {}
	HookPointer(const HookPointer<T> &that) : m_obj(that.m_obj), m_ptr(that.m_ptr) {}
	~HookPointer() {}

	HookPointer &operator =(const HookPointer<T> &that)
	{
		m_obj = that.m_obj;
		m_ptr = that.m_ptr;
		return *this;
	}
	HookPointer &operator =(QObject *obj) { return operator =(HookPointer(obj)); }

	T *operator ->() { return m_ptr; }
	operator QObject *() const { return m_obj; }
	operator T *() const { return m_ptr; }
	operator bool() const { return m_obj != NULL; }

	inline QObject *object() const { return m_obj; }
	inline T *pointer() const { return m_ptr; }
private:
	QObject *m_obj;
	T *m_ptr;
};

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
	QList<HookPointer<PlayerFactory> > playerFactories() { return m_playerFactories; }
	QHash<QString, HookPointer<Player> > players() { return m_players; }
		QString currentPlayerId() { return m_playerId; }
	Player *currentPlayer() { return m_player; }
	bool forAllAccounts() { return m_forAllAccounts; }
	bool isWorking() { return m_isWorking; }
	static NowPlaying *instance() { Q_ASSERT(self); return self; }
	virtual bool eventFilter(QObject *obj, QEvent *ev);
public slots:
	void loadSettings();
	void setState(bool isWorking);
private slots:
	void stopStartActionTrigged();
	void playingStatusChanged(bool);
	void accountCreated(qutim_sdk_0_3::Account*);
	void accountDeleted(QObject*);
	void setStatuses(const TrackInfo &info);
private:
	void initPlayer(const QString &playerName, PlayerFactory *factory = 0);
	void clearStatuses();
private:
	StopStartActionGenerator* m_stopStartAction;
	QList<HookPointer<PlayerFactory> > m_playerFactories;
	QHash<QString, HookPointer<Player> > m_players;
	HookPointer<Player> m_player;
	QString m_playerId;
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

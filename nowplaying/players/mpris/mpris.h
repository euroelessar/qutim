#ifndef MPRIS_H
#define MPRIS_H

#include "player.h"
#include "playersettings.h"
#include <QtDBus>
#include <QTimer>

struct DBusStatus
{
    int Play;
    int Random;
    int Repeat;
    int RepeatPlaylist;
};

Q_DECLARE_METATYPE(DBusStatus)

class QComboBox;

namespace qutim_sdk_0_3 {

namespace nowplaying {

	class Mpris;

	class MprisSettings : public PlayerSettings
	{
	public:
		MprisSettings(Mpris *mpris);
		virtual void saveSettings();
		virtual void loadSettings();
	private:
		QComboBox *m_comboBox;
		Mpris *m_mpris;
	};

	class Mpris : public Player
	{
		Q_OBJECT
		Q_INTERFACES(qutim_sdk_0_3::nowplaying::Player)
	public:
		Mpris();
		QString playerName();
		TrackInfo trackInfo();
		void init();
		bool isPlaying();
		void startWatching();
		void stopWatching();
		PlayerSettings *settings();
		QString currentPlayer() { return m_currentPlayer; }
	public slots:
		void trackChanged();
		void statusChanged(const DBusStatus &status);
	private:
		bool getIsPlaying();
		bool m_isPlaying;
		QString m_currentPlayer;
		QScopedPointer<QDBusInterface> m_dbusInterface;

	};

} }

#endif // MPRIS_H

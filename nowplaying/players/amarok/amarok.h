#ifndef AMAROK_H
#define AMAROK_H

#include "../../include/player.h"
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

namespace qutim_sdk_0_3 {

namespace nowplaying {

	class Amarok : public Player
	{
		Q_OBJECT
		Q_INTERFACES(qutim_sdk_0_3::nowplaying::Player)
	public:
		QString playerName();
		TrackInfo trackInfo();
		void init();
		bool isPlaying();
		void startWatching();
		void stopWatching();
	private:
		bool m_is_playing;
		bool m_track_changed;
		int m_count;
		QDBusInterface* m_dbus_interface;
		QTimer* m_timer;
	public slots:
		void trackChanged();
		void statusChanged(DBusStatus);
		void isPlayingCheck();
	};

} }

#endif // AMAROK_H

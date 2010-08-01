#include "amarok.h"
#include <QVariantMap>
#include <QFileInfo>
#include <QTime>

namespace qutim_sdk_0_3 {
namespace nowplaying{

    QString Amarok::playerName(){
        return QString("Amarok");
    }

     TrackInfo Amarok::trackInfo(){
        TrackInfo track_info;
        QDBusReply<QVariantMap> reply = m_dbus_interface->call("GetMetadata");
        track_info.album = reply.value().value("album").toString();
        track_info.artist = reply.value().value("artist").toString();
        track_info.title = reply.value().value("title").toString();
        track_info.uri = reply.value().value("location").toString();
        track_info.uri.replace("%20", " ");
        track_info.file_name = QFileInfo(track_info.uri).baseName();
        track_info.track_number = reply.value().value("tracknumber").toString();
        track_info.time = QTime().addSecs(reply.value().value("time").toLongLong()).toString("H:mm:ss");
        return track_info;
    }

     bool Amarok::isPlaying(){
        QDBusReply<DBusStatus> reply = m_dbus_interface->call("GetStatus");
        return reply.value().Play == 0;
    }

    void Amarok::startWatching(){
        qDBusRegisterMetaType<DBusStatus>();
        m_dbus_interface = new QDBusInterface("org.mpris.amarok", "/Player", "org.freedesktop.MediaPlayer", QDBusConnection::sessionBus(), this);
        m_is_playing = isPlaying();
        m_dbus_interface->connection().connect("org.mpris.amarok",
                                               "/Player",
                                               "org.freedesktop.MediaPlayer",
                                               "TrackChange",
                                               this,
                                               SLOT(trackChanged())
                                               );
        /*m_dbus_interface->connection().connect("org.mpris.amarok",
                                               "/Player",
                                               "org.freedesktop.MediaPlayer",
                                               "StatusChange",
                                               this,
                                               SLOT(statusChanged(DBusStatus))
                                               );
          */
        m_timer = new QTimer(this);
        connect (m_timer, SIGNAL(timeout()), this, SLOT(isPlayingCheck()));
        m_timer->start(1000);
    }

    void Amarok::stopWatching(){
        delete m_dbus_interface;
        delete m_timer;
    }

    void Amarok::trackChanged(){
        TrackInfo info = trackInfo();
        if (!info.uri.isEmpty() && info.time != "0:00:00"){
            emit Player::trackChanged(info);
        }
        m_track_changed = true;
        m_count = 0;
    }

    /*
    void Amarok::statusChanged(DBusStatus status){
        if (m_track_changed){
            m_count++;
            if (m_count < 7){
                return;
            } else{
                m_track_changed = false;
            }
        }
        bool is_playing = status.Play == 0;
        if (m_is_playing && !is_playing){
            m_is_playing = false;
            emit playingStatusChanged(false);
        } else if (!m_is_playing && is_playing){
            m_is_playing = true;
            emit playingStatusChanged(true);
        }
    }*/

    void Amarok::isPlayingCheck(){
        bool is_playing = isPlaying();
        if (m_is_playing && !is_playing){
            m_is_playing = false;
            emit playingStatusChanged(false);
        } else if (!m_is_playing && is_playing){
            m_is_playing = true;
            emit playingStatusChanged(true);
        }
    }
}
}

const QDBusArgument & operator<<(QDBusArgument &arg, const DBusStatus& status) {
    arg.beginStructure();
    arg << status.Play << status.Random << status.Repeat << status.RepeatPlaylist;
    arg.endStructure();
    return arg;
}

const QDBusArgument & operator>>(const QDBusArgument& arg, DBusStatus& status) {
    arg.beginStructure();
    arg >> status.Play >> status.Random >> status.Repeat >> status.RepeatPlaylist;
    arg.endStructure();
    return arg;
}

#include "amarok.h"

Amarok::Amarok(quint32 update_interval = 1000, QObject* parent = 0) : Player(update_interval, parent){
    m_stopped = true;
    m_dbus_interface = new QDBusInterface("org.mpris.amarok", "/Player", QString(), QDBusConnection::sessionBus(), this);
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

Amarok::~Amarok(){

}

TrackInfo Amarok::getTrackInfo(){
    return m_track_info;
}

bool Amarok::isPlayingStopped(){
    return m_stopped;
}

void Amarok::timeout(){
   QDBusReply<QVariantMap> reply = m_dbus_interface->call("GetMetadata");
   bool track_changed = true;
   if (reply.isValid()){
       QString uri(reply.value().value("location").toString());
       clearURI(uri);
       if (m_track_info.uri == uri){
           if (m_track_info.artist == reply.value().value("artist").toString()){
               if (m_track_info.title == reply.value().value("title").toString()){
                   if (m_track_info.album == reply.value().value("album").toString()){
                       track_changed = false;
                   }
               }
           }
       }
   } else{
       track_changed = false;
   }
   if (track_changed){
       m_track_info.album = reply.value().value("album").toString();
       m_track_info.artist = reply.value().value("artist").toString();
       m_track_info.title = reply.value().value("title").toString();
       m_track_info.uri = reply.value().value("location").toString();
       clearURI(m_track_info.uri);
       m_track_info.file_name = getFileNameFromURI(m_track_info.uri);
       m_track_info.track_number = reply.value().value("tracknumber").toString();
       m_track_info.time = convertSeconds(reply.value().value("time").toUInt());
       emit trackChanged(m_track_info);
   }
   QDBusReply<int> position_1 = m_dbus_interface->call("PositionGet");
   QTest::qWait(1000);
   QDBusReply<int> position_2 = m_dbus_interface->call("PositionGet");
   if (!(position_2 - position_1)){
       m_stopped = true;
       emit playingStopped(m_stopped);
   } else if (m_stopped){
       m_stopped = false;
       emit playingStopped(m_stopped);
   }
}

void Amarok::startWatching(){
    timeout();
    if (!m_timer->isActive()){
        m_timer->start(m_update_interval);
    }
}

void Amarok::stopWatching(){
    if (m_timer->isActive()){
        m_timer->stop();
    }
}

void Amarok::changeUpdatePeriod(quint32 update_period){
    m_update_interval = update_period;
    stopWatching();
    startWatching();
}

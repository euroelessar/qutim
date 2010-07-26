#include "player.h"

Player::Player(quint32 update_interval = 1000, QObject *parent = 0) : QObject(parent){
    m_update_interval = update_interval;
}

Player::~Player(){

}

QString Player::convertSeconds(quint32 secs){
    QString time;
    if (secs < 60){
        time = QString::number(secs) + "s";
    } else if (secs < 360){
        quint32 min = secs/60;
        quint32 sec = secs - min*60;
        time = QString::number(min) + "m " + QString::number(sec) + "s";
    } else if (secs < 23040){
        quint32 h = secs/360;
        quint32 min = (secs - h*360) / 60;
        quint32 sec = secs - (h*360 + min*60);
        time = QString::number(h) + "h " + QString::number(min) + "m " + QString::number(sec) + "s";
    } else {
        quint32 d = secs/23040;
        quint32 h = (secs - d*23040) / 360;
        quint32 min = (secs - d*23040 - h*360) / 60;
        quint32 sec = secs - (d*23040 + h*360 + min*60);
        time = QString::number(d) + "d " + QString::number(h) + "h " + QString::number(min) + "m " + QString::number(sec) + "s";
    }
    return time;
}

QString Player::getFileNameFromURI(QString URI){
   clearURI(URI);
   QFileInfo file(URI);
   return file.baseName();
}

void Player::clearURI(QString& URI){
    URI.replace( "%20", " " );
}


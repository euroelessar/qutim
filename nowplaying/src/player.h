#ifndef PLAYER_H
#define PLAYER_H

#include "trackinfo.h"
#include <QObject>
#include <QFileInfo>

class Player : public QObject
{
    Q_OBJECT
public:
    Player(quint32 update_interval, QObject *parent);
    quint32 m_update_interval;
    virtual ~Player();
    virtual TrackInfo getTrackInfo() = 0;
    virtual bool isPlayingStopped() = 0;
    virtual void startWatching() = 0;
    virtual void stopWatching() = 0;
    virtual void changeUpdatePeriod(quint32) = 0;

protected:
    QString convertSeconds(quint32 secs);
    QString getFileNameFromURI(QString URI);
    void clearURI(QString& URI);
signals:
    void playingStopped(bool);
    void trackChanged(const TrackInfo&);
};

#endif // PLAYER_H

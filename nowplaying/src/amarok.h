#ifndef AMAROK_H
#define AMAROK_H

#include "player.h"
#include <QTimer>
#include <QDBusInterface>
#include <QDBusReply>
#include <QVariantMap>
#include <QDebug>
#include <QtTest/QTest>

class Amarok : public Player{
    Q_OBJECT
public:
    Amarok(quint32 update_interval, QObject* parent);
    ~Amarok();
    TrackInfo getTrackInfo();
    bool isPlayingStopped();
    void startWatching();
    void stopWatching();
    void changeUpdatePeriod(quint32);

protected:
    TrackInfo m_track_info;
    bool m_stopped;
    QTimer* m_timer;
    QDBusInterface* m_dbus_interface;

protected slots:
    void timeout();
};

#endif // AMAROK_H

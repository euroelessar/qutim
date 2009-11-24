/****************************************************************************
 *  mrimconnection.h
 *
 *  Copyright (c) 2009 by Rusanov Peter <peter.rusanov@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef MRIMCONNECTION_H
#define MRIMCONNECTION_H

#include <QObject>
#include <QScopedPointer>

#include <qutim/configbase.h>

using namespace qutim_sdk_0_3;

class MrimAccount;
struct MrimConnectionPrivate;

class MrimConnection : public QObject
{
    Q_OBJECT

public:
    enum TConnectionState
    {
        Unconnected = 0,
        RecievingGoodIMServer = 1,
        ConnectingToIMServer = 2,
        ConnectedToIMServer = 3,
        UnknownState = 4
    };

public:
    MrimConnection(MrimAccount *account);
    void start();
    void close();
    ~MrimConnection();
    Config config();
    ConfigGroup config(const QString &group);
    TConnectionState state() const;

private slots:
    void connected();
    void readyRead();

private:
    Q_DISABLE_COPY(MrimConnection)
    QScopedPointer<MrimConnectionPrivate> p;
};

#endif // MRIMCONNECTION_H

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Rusanov Peter <peter.rusanov@gmail.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef MRIMCONNECTION_H
#define MRIMCONNECTION_H

#include <QObject>
#include <QScopedPointer>

#include <qutim/configbase.h>
#include <qutim/status.h>
#include <qutim/message.h>

#include "mrimpacket.h"
#include "messages.h"

using namespace qutim_sdk_0_3;

class MrimAccount;
class MrimContact;
struct MrimConnectionPrivate;

class MrimConnection : public QObject,
                       public PacketHandler
{
    Q_OBJECT

public:
    enum ConnectionState
    {
        Unconnected = 0,
        RecievingGoodIMServer = 1,
        ConnectingToIMServer = 2,
        ConnectedToIMServer = 3,
        UnknownState = 4
    };

    enum FeatureFlag
    {
        FeatureFlagRtfMessage       =   0x00000001,
        FeatureFlagBaseSmiles       =   0x00000002,
        FeatureFlagAdvancedSmiles   =   0x00000004,
        FeatureFlagContactsExchange =   0x00000008,
        FeatureFlagWakeup           =   0x00000010,
        FeatureFlagMults            =   0x00000020,
        FeatureFlagFileTransfer     =   0x00000040,
        FeatureFlagVoice            =   0x00000080,
        FeatureFlagVideo            =   0x00000100,
        FeatureFlagGames            =   0x00000200,
        FeatureFlagLast             =   FeatureFlagGames,
        FeatureFlagUAMask           =   ((FeatureFlagLast << 1) - 1)
    };
    Q_DECLARE_FLAGS(FeatureFlags,FeatureFlag)

public:
    MrimConnection(MrimAccount *account);
    void start();
    void close();
    virtual ~MrimConnection();

    Config config();
    ConfigGroup config(const QString &group);
    ConnectionState state() const;
    void registerPacketHandler(PacketHandler *handler);
    FeatureFlags protoFeatures() const;
	Status setStatus(const Status &status);

    void sendPacket(MrimPacket &packet);
    MrimAccount *account() const;
    MrimMessages *messages() const;

signals:
    void loggedOut(); //please do a queued connection
    void loggedIn(); //please do a queued connection

public slots:

protected slots:
    void connected();
    void disconnected();
    void readyRead();
    void sendPing();

protected:
    virtual bool processPacket();
	void sendStatusPacket();
    virtual void sendGreetings();
    virtual void login();
    virtual QList<quint32> handledTypes();
    virtual bool handlePacket(class MrimPacket& packet);
    virtual void loginRejected(const QString& reason);

private:
    Q_DISABLE_COPY(MrimConnection)
    QScopedPointer<MrimConnectionPrivate> p;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MrimConnection::FeatureFlags)

#endif // MRIMCONNECTION_H


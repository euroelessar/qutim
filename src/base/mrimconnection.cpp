/****************************************************************************
 *  mrimconnection.cpp
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

#include <QStringList>
#include <QTcpSocket>
#include <QHostAddress>
#include <QTimer>
#include <QMap>
#include <QApplication>

#include <qutim/notificationslayer.h>

#include "proto.h"
#include "utils.h"
#include "mrimdebug.h"
#include "mrimaccount.h"
#include "mrimpacket.h"
#include "useragent.h"

#include "mrimconnection.h"

typedef QMap<quint32,PacketHandler*> QHandlersMap;

struct MrimConnectionPrivate
{
    MrimConnectionPrivate(MrimAccount *acc)
        : account(acc), imSocket(new QTcpSocket), srvReqSocket(new QTcpSocket), readyReadTimer(new QTimer),
          pingTimer(new QTimer)
    {
        readyReadTimer->setSingleShot(true);
        readyReadTimer->setInterval(0);

    }

    inline QTcpSocket *IMSocket() const     { return imSocket.data(); }
    inline QTcpSocket *SrvReqSocket() const { return srvReqSocket.data(); }
    inline QTimer *ReadyReadTimer() const   { return readyReadTimer.data(); }

    QString imHost;
    quint32 imPort;
    MrimAccount *account;
    MrimPacket   readPacket;
    UserAgent    selfID;

    QScopedPointer<QTcpSocket> imSocket;
    QScopedPointer<QTcpSocket> srvReqSocket;
    QScopedPointer<QTimer> readyReadTimer;
    QScopedPointer<QTimer> pingTimer;
    QHandlersMap handlers;
    QList<quint32> handledTypes;
};

MrimConnection::MrimConnection(MrimAccount *account) : p(new MrimConnectionPrivate(account))
{    
    connect(p->SrvReqSocket(),SIGNAL(connected()),this,SLOT(connected()));
    connect(p->SrvReqSocket(),SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(p->SrvReqSocket(),SIGNAL(readyRead()),this,SLOT(readyRead()));
    connect(p->IMSocket(),SIGNAL(connected()),this,SLOT(connected()));
    connect(p->IMSocket(),SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(p->IMSocket(),SIGNAL(readyRead()),this,SLOT(readyRead()));
    connect(p->ReadyReadTimer(),SIGNAL(timeout()),this,SLOT(readyRead()));
    connect(p->pingTimer.data(),SIGNAL(timeout()),this,SLOT(sendPing()));
    registerPacketHandler(this);
    UserAgent qutimAgent(QApplication::applicationName(),QApplication::applicationVersion(),
                         "(git)",PROTO_VERSION_MAJOR,PROTO_VERSION_MINOR); //TODO: real build version
    p->selfID.set(qutimAgent);
}

MrimConnection::~MrimConnection()
{
    p->SrvReqSocket()->disconnect(this);
    p->IMSocket()->disconnect(this);
    p->ReadyReadTimer()->disconnect(this);
    p->pingTimer->disconnect(this);
    close();
}

void MrimConnection::start()
{
    Q_ASSERT(state() == Unconnected);
    QString srvReqHost = config("connection").value("reqSrvHost",QString("mrim.mail.ru"));
    quint32 srvReqPort = config("connection").value("reqSrvPort",2042);
    p->srvReqSocket->connectToHost(srvReqHost,srvReqPort,QAbstractSocket::ReadOnly);
}

void MrimConnection::close()
{
    if (p->IMSocket()->isOpen())
    {
        p->IMSocket()->disconnectFromHost();
    }

    if (p->SrvReqSocket()->isOpen())
    {
        p->IMSocket()->disconnectFromHost();
    }
}

void MrimConnection::connected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    Q_ASSERT(socket);

    bool connected = false;

    switch (socket->state())
    {
    case QAbstractSocket::ConnectedState:
        connected = true;
        break;
    default:
        break;
    }

    QString address = Utils::toHostPortPair(socket->peerAddress(),socket->peerPort());

    if (!connected)
    {
        debug()<<"Connection to server"<<qPrintable(address)<<"failed! :(";
        return;
    }
    else
    {
        debug()<<"Connected to server"<<qPrintable(address);

        if (socket == p->IMSocket()) //temp
        {
            sendGreetings();
        }
    }
}

void MrimConnection::disconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    Q_ASSERT(socket);

    debug()<<"Disconnected from server"<<qPrintable( Utils::toHostPortPair(socket->peerAddress(),socket->peerPort()) );

    if (socket == p->SrvReqSocket())
    {
        if (!p->imHost.isEmpty() && p->imPort > 0)
        {//all is fine, connecting to IM server
            p->IMSocket()->connectToHost(p->imHost,p->imPort);
        }
        else
        {
            critical()<<"Oh god! This is epic fail! We didn't recieve any server, so connection couldn't be established!";
        }
    }
}

Config MrimConnection::config()
{
    return p->account->config();
}

ConfigGroup MrimConnection::config(const QString &group)
{
    return p->account->config(group);
}

QList<quint32> MrimConnection::handledTypes()
{
    if (p->handledTypes.isEmpty())
    {
        p->handledTypes << MRIM_CS_HELLO_ACK
                        << MRIM_CS_LOGIN_ACK
                        << MRIM_CS_LOGIN_REJ
                        << MRIM_CS_LOGOUT;
    }
    return p->handledTypes;
}

bool MrimConnection::handlePacket(MrimPacket& packet)
{
    bool handled = true;

    switch (packet.msgType())
    {
    case MRIM_CS_HELLO_ACK:
        {
            quint32 pingTimeout = 0;
            packet.readTo(pingTimeout);

            if (p->pingTimer->isActive())
            {
                p->pingTimer->stop();
            }
            p->pingTimer->setInterval(pingTimeout);
            login();
        }
        break;
    case MRIM_CS_LOGIN_ACK:
        p->pingTimer->start();
        break;
    case MRIM_CS_LOGIN_REJ:
        Notifications::sendNotification(Notifications::System,p->account,tr("Authentication failed!"));
        break;        
    case MRIM_CS_LOGOUT:
        {
            quint32 reason = 0;
            packet.readTo(reason);

            if (reason == LOGOUT_NO_RELOGIN_FLAG)
            {
                Notifications::sendNotification(Notifications::System,p->account,tr("Another client with same login connected!"));
                //TODO: do not relogin
            }
        }
        break;
    default:
        handled = false;
        break;
    }
    return handled;
}

MrimConnection::ConnectionState MrimConnection::state() const
{
    QAbstractSocket::SocketState srvReqState =  p->SrvReqSocket()->state();
    QAbstractSocket::SocketState IMState =  p->IMSocket()->state();

    if (srvReqState == QAbstractSocket::UnconnectedState &&
        IMState == QAbstractSocket::UnconnectedState)
    {
        return Unconnected;
    }

    switch (srvReqState)
    {
    case QAbstractSocket::ConnectingState:
    case QAbstractSocket::ConnectedState:
        return RecievingGoodIMServer;
    }

    switch (IMState)
    {
    case QAbstractSocket::ConnectingState:
        return ConnectingToIMServer;
    case QAbstractSocket::ConnectedState:
        return ConnectedToIMServer;        
    }

    return UnknownState;
}

void MrimConnection::readyRead()
{    
    QTcpSocket *socket = (p->IMSocket()->bytesAvailable()) ? p->IMSocket() : p->SrvReqSocket();
    Q_ASSERT(socket);

    if (socket->bytesAvailable() <= 0) //windows hack?
    {
        return;
    }

    if (socket == p->SrvReqSocket())
    {//it can contain only IP:Port pair
        QStringList ipPortPair = QString(socket->readAll()).split(':');
        p->imHost = ipPortPair[0];
        p->imPort = ipPortPair[1].toUInt();
        //srv request socket will disconnected immediatly
    }
    else
    {
        if (p->readPacket.readFrom(*socket))
        {
            if (p->readPacket.isFinished())
            {
                processPacket();
                p->readPacket.clear();
            }
        }
        else
        {
            close();
        }

        if (p->readPacket.lastError() != MrimPacket::NoError)
        {
            debug(Verbose)<<"Error while reading packet:" << p->readPacket.lastErrorString() ;
        }
    }

    if (socket->bytesAvailable())
    {//run next read round
        p->readyReadTimer->start();
    }
}

bool MrimConnection::processPacket()
{
    Q_ASSERT(p->readPacket.isFinished());
    debug(VeryVerbose)<<"Recieved packet of type"<<hex<<p->readPacket.msgType();

    bool handled = false;
    QHandlersMap::iterator it = p->handlers.find(p->readPacket.msgType());

    if (it != p->handlers.end())
    {
        handled = it.value()->handlePacket(p->readPacket);
    }

    if (!handled)
    {
        debug(VeryVerbose)<<"Packet was not handled!";
        debug(VeryVerbose)<<p->readPacket;
    }
}

 void MrimConnection::registerPacketHandler(PacketHandler *handler)
 {
    Q_ASSERT(handler);
    QList<quint32> msgTypes = handler->handledTypes();

    foreach (quint32 type, msgTypes)
    {
        p->handlers[type] = handler;
    }
 }

MrimConnection::FeatureFlags MrimConnection::protoFeatures() const
{
    static FeatureFlags supportedFeatures = FeatureFlagBaseSmiles;
    return supportedFeatures;
}

void MrimConnection::sendGreetings()
{
    MrimPacket hello(MrimPacket::Compose);
    hello.setMsgType(MRIM_CS_HELLO);
    hello.setBody("");
    hello.writeTo(p->IMSocket());
}

void MrimConnection::login()
{
    MrimPacket login(MrimPacket::Compose);
    login.setMsgType(MRIM_CS_LOGIN2);
    login << p->account->id();
    login << config("general").value("passwd",QString(),Config::Crypted);
    login << STATUS_ONLINE; //TODO: TEMP!!!
    login << "STATUS_ONLINE"; //TODO: TEMP!!!
    login.append("Online",true); //TODO: TEMP!!!
    login.append("",true); //TODO: TEMP!!!
    login << protoFeatures();
    login << p->selfID.toString();
    login << "ru"; //TODO: TEMP !!
#if PROTO_VERSION_MINOR >= 20
    //hack for 1.20
    login << 0; //NULL
    login << 0; //NULL
#endif
    login << QString("%1 %2;").arg(QApplication::applicationName()).arg(QApplication::applicationVersion());
    login.writeTo(p->IMSocket());
}

void MrimConnection::sendPing()
{
    if (state() != ConnectedToIMServer)
    {
        p->pingTimer->stop();
        return;
    }

    MrimPacket ping(MrimPacket::Compose);
    ping.setMsgType(MRIM_CS_PING);
    ping.setBody("");
    ping.writeTo(p->IMSocket());
}

bool MrimConnection::setStatus(Status status)
{
    //TODO:


}

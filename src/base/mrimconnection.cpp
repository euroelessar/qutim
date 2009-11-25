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

#include "utils.h"
#include "mrimdebug.h"
#include "mrimaccount.h"
#include "mrimpacket.h"

#include "mrimconnection.h"

struct MrimConnectionPrivate
{
    MrimConnectionPrivate(MrimAccount *acc)
        : account(acc), imSocket(new QTcpSocket), srvReqSocket(new QTcpSocket), readyReadTimer(new QTimer)
    {
        readyReadTimer->setSingleShot(true);
        readyReadTimer->setInterval(0);
    }

    inline QTcpSocket *IMSocket() const     { return imSocket.data(); }
    inline QTcpSocket *SrvReqSocket() const { return srvReqSocket.data(); }
    inline QTimer *ReadyReadTimer() const   { return readyReadTimer.data(); }

    MrimAccount *account;
    QScopedPointer<QTcpSocket> imSocket;
    QScopedPointer<QTcpSocket> srvReqSocket;
    QString imHost;
    quint32 imPort;
    QScopedPointer<QTimer> readyReadTimer;
    MrimPacket m_readPacket;
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
}

MrimConnection::~MrimConnection()
{
    p->SrvReqSocket()->disconnect(this);
    p->IMSocket()->disconnect(this);
    p->ReadyReadTimer()->disconnect(this);
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
        MDEBUG(Info,"Connection to Mail.ru server at "<<qPrintable(address)<<" has failed! :(");
        return;
    }
    else
    {
        MDEBUG(Info,"Connected to Mail.ru server at"<<qPrintable(address));

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

    MDEBUG(Info,"Disconnected from Mail.ru server at "<<qPrintable( Utils::toHostPortPair(socket->peerAddress(),socket->peerPort()) ) );

    if (socket == p->SrvReqSocket())
    {
        if (!p->imHost.isEmpty() && p->imPort > 0)
        {//all is fine, connecting to IM server
            p->IMSocket()->connectToHost(p->imHost,p->imPort);
        }
        else
        {
            MCRIT("Oh god! This is epic fail! We didn't recieve any server, connection couldn't be established!");
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

MrimConnection::TConnectionState MrimConnection::state() const
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
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());

    if (!socket)
    {
        if (qobject_cast<QTimer*>(sender()) != 0)
        {
            socket = (p->IMSocket()->bytesAvailable()) ? p->IMSocket() : p->SrvReqSocket();
        }
    }

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
        if (p->m_readPacket.readFrom(*socket))
        {
            if (p->m_readPacket.isFinished())
            {
                processPacket();
                p->m_readPacket.clear();
            }
        }
        else
        {
            close();
        }

        if (p->m_readPacket.lastError() != MrimPacket::NoError)
        {
            MDEBUG(Verbose,"An error occured while reading packet:" << p->m_readPacket.lastErrorString() );
        }
    }

    if (socket->bytesAvailable())
    {//run next read round
        p->readyReadTimer->start();
    }
}

bool MrimConnection::processPacket()
{
    Q_ASSERT(p->m_readPacket.isFinished());
    //TODO: packets processing
}

void MrimConnection::sendGreetings()
{
    MrimPacket hello(MrimPacket::Compose);
    hello.setMsgType(MRIM_CS_HELLO);
    hello.setBody("");
    hello.writeTo(p->IMSocket());
}

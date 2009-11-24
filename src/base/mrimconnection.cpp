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

#include "mrimdebug.h"
#include "mrimaccount.h"
#include "mrimconnection.h"

struct MrimConnectionPrivate
{
    MrimConnectionPrivate(MrimAccount *acc)
        : account(acc), imSocket(new QTcpSocket), srvReqSocket(new QTcpSocket)
    { }

    inline QTcpSocket *IMSocket() const     { return imSocket.data(); }
    inline QTcpSocket *SrvReqSocket() const { return srvReqSocket.data(); }

    MrimAccount *account;
    QScopedPointer<QTcpSocket> imSocket;
    QScopedPointer<QTcpSocket> srvReqSocket;
    QString imHost;
    quint32 imPort;
};

MrimConnection::MrimConnection(MrimAccount *account) : p(new MrimConnectionPrivate(account))
{    
    connect(p->SrvReqSocket(),SIGNAL(connected()),this,SLOT(connected()));
    connect(p->SrvReqSocket(),SIGNAL(readyRead()),this,SLOT(readyRead()));
    connect(p->IMSocket(),SIGNAL(connected()),this,SLOT(connected()));
    connect(p->IMSocket(),SIGNAL(readyRead()),this,SLOT(readyRead()));
}

MrimConnection::~MrimConnection()
{
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

    QString address = QString("%1:%2").arg(socket->peerAddress().toString()).arg(socket->peerPort());

    if (!connected)
    {
        MDEBUG(Info,"Connection to Mail.ru server server at "<<qPrintable(address)<<" has failed! :(");
        return;
    }
    else
    {
        MDEBUG(Info,"Connected to Mail.ru server at"<<qPrintable(address));
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
    Q_ASSERT(socket);

    if (socket->bytesAvailable() == 0) //hack
    {
        return;
    }

    if (socket == p->SrvReqSocket())
    {//it can contain only IP:Port pair
        QStringList ipPortPair = QString(socket->readAll()).split(':');
        p->imHost = ipPortPair[0];
        p->imPort = ipPortPair[1].toUInt();
        //TODO: handle further connection
    }
}

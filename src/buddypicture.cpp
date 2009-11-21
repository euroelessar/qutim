/****************************************************************************
 *  buddypicture.cpp
 *
 *  Copyright (c) 2009 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "buddypicture.h"
#include "icqaccount.h"
#include <QSet>
#include <QDebug>
#include <QImage>

BuddyPictureConnection::BuddyPictureConnection(QObject *parent):
	AbstractConnection(parent)
{
}

void BuddyPictureConnection::connectToServ(const QString &addr, quint16 port, const QByteArray &cookie)
{
	m_cookie = cookie;
	socket()->connectToHost(addr, port);
}

void BuddyPictureConnection::processNewConnection()
{
	FLAP flap(0x01);
	flap.appendSimple<quint32>(0x01);
	flap.appendTLV<QByteArray>(0x0006, m_cookie);
	send(flap);
}

void BuddyPictureConnection::processCloseConnection()
{
	AbstractConnection::processCloseConnection();
}

BuddyPicture::BuddyPicture(IcqAccount *account, QObject *parent):
	ProtocolNegotiation(parent), m_account(account), m_is_connected(false)
{
	m_infos << SNACInfo(ServiceFamily, ServerRedirectService)
			<< SNACInfo(AvatarFamily, AvatarGetReply);
	m_conn = new BuddyPictureConnection(this);
	m_conn->registerHandler(this);
	connect(m_conn->socket(), SIGNAL(disconnected()), SLOT(disconnected()));
}

BuddyPicture::~BuddyPicture()
{

}

void BuddyPicture::handleSNAC(AbstractConnection *conn, const SNAC &snac)
{
	if(conn == m_conn)
	{
		ProtocolNegotiation::handleSNAC(conn, snac);
		snac.resetState();
		if(snac.family() == ServiceFamily && snac.subtype() == ServiceServerRateInfo)
		{
			SNAC snac(ServiceFamily, ServiceClientReady);
			snac.appendData(QByteArray::fromHex(
					"0001 0004 0110 164f"   // ServiceFamily
					"000f 0001 0110 164f"));// AvatarFamily
			conn->send(snac);
			m_is_connected = true;
			while(!m_history.isEmpty())
			{
				SNAC tmp = m_history.takeFirst();
				conn->send(tmp);
			}
		}
	}
	else
	{
		switch((snac.family() << 16) | snac.subtype())
		{
			case ServiceFamily << 16 | ServerRedirectService: {
				TLVMap tlvs = snac.readTLVChain();
				quint16 id = tlvs.value(0x0D).value<quint16>();
				if(id == AvatarFamily)
				{
					QList<QByteArray> list = tlvs.value(0x05).value().split(':');
					m_conn->connectToServ(list.at(0), list.size() > 1 ? atoi(list.at(1).constData()) : 5190, tlvs.value(0x06).value());
				}
			}
			break;
			default:{

			}
		}
	}
	switch((snac.family() << 16) | snac.subtype())
	{
		case AvatarFamily << 16 | AvatarGetReply: {
			QString uin = snac.readString<quint8>();
			qDebug() << "avatar update for" << uin;
			ChatUnit *contact = m_account->getUnit(uin, false);
			if(!contact)
				break;
			snac.skipData(3); // skip icon_id and icon_flag
			QByteArray hash = snac.readData<quint8>();
			snac.skipData(21);
			QImage image = QImage::fromData(snac.readData<quint16>());
			contact->setProperty("icon_hash", hash);
			contact->setProperty("avatar", image);
		}
		break;
		default:{

		}
	}
}

void BuddyPicture::sendUpdatePicture(IcqContact *contact, quint16 icon_id, quint8 icon_flags, const QByteArray &icon_hash)
{
	if(m_conn->socket()->state() == QTcpSocket::UnconnectedState)
		return;
	QByteArray old_hash = contact->property("icon_hash").toByteArray();
	if(old_hash != icon_hash)
	{
		SNAC snac(AvatarFamily, AvatarGetRequest);
		snac.appendData<quint8>(contact->id());
		snac.appendSimple<quint8>(1); // unknown
		snac.appendSimple<quint16>(icon_id);
		snac.appendSimple<quint8>(icon_flags);
		snac.appendData<quint8>(icon_hash);
		if(m_is_connected)
			m_conn->send(snac);
		else
			m_history.push_back(snac);
	}
}

void BuddyPicture::disconnected()
{
	m_is_connected = false;
	m_history.clear();
}

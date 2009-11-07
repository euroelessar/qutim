/****************************************************************************
 *  md5login.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "md5login.h"
#include "tlv.h"
#include "icqaccount.h"
#include <QCryptographicHash>
#include <QUrl>

Md5Login::Md5Login()
{
	m_infos << SNACInfo(AuthorizationFamily, SignonLoginReply)
			<< SNACInfo(AuthorizationFamily, SignonAuthKey);
}

void Md5Login::handleSNAC(OscarConnection *conn, const SNAC &sn)
{
	if(sn.subtype() == SignonAuthKey)
	{
		const ClientInfo &client = conn->clientInfo();
		SNAC snac(AuthorizationFamily, SignonLoginRequest);
		snac.setId(qrand());
		snac.appendTLV<QByteArray>(0x0001, conn->account()->id().toUtf8());
		{
			quint32 length = qFromBigEndian<quint32>((uchar *)sn.data().constData());
			QByteArray key = sn.data().mid(2, length);
			QString password = conn->account()->config().group("general").value("passwd", QString(), Config::Crypted);
			key += QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5);;
			key += "AOL Instant Messenger (SM)";
			snac.appendTLV(0x0025, QCryptographicHash::hash(key, QCryptographicHash::Md5));
		}
		// Flag for "new" md5 authorization
		snac.appendData(TLV(0x004c));
		snac.appendTLV<QByteArray>(0x0003, client.id_string);
		snac.appendTLV<quint16>(0x0017, client.major_version);
		snac.appendTLV<quint16>(0x0018, client.minor_version);
		snac.appendTLV<quint16>(0x0019, client.lesser_version);
		snac.appendTLV<quint16>(0x001a, client.build_number);
		snac.appendTLV<quint16>(0x0016, client.id_number);
		snac.appendTLV<quint32>(0x0014, client.distribution_number);
		snac.appendTLV<QByteArray>(0x000f, client.language);
		snac.appendTLV<QByteArray>(0x000e, client.country);
		// Unknown shit
		snac.appendTLV<quint8>(0x0094, 0x00);
		conn->send(snac);
	}
	else if(sn.subtype() == SignonLoginReply)
	{
		TLVMap tlvs = sn.readTLVChain();
		if(tlvs.contains(0x01) && tlvs.contains(0x05) && tlvs.contains(0x06))
		{
			QList<QByteArray> list = tlvs.value(0x05).value().split(':');
			conn->connectToBOSS(list.at(0), list.size() > 1 ? atoi(list.at(1).constData()) : 5190, tlvs.value(0x06).value());
		}
		else
		{
			qDebug() << Util::connectionErrorText(qFromBigEndian<quint16>((const uchar *)tlvs.value(0x08).value().constData()));
		}
	}
}

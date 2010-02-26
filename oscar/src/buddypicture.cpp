/****************************************************************************
 *  buddypicture.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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
#include "qutim/systeminfo.h"
#include "qutim/protocol.h"
#include <qutim/debug.h>
#include "icqaccount.h"
#include <QSet>
#include <QDir>
#include <QFile>

#include <QImage>

namespace qutim_sdk_0_3 {

namespace oscar {

BuddyPicture::BuddyPicture(IcqAccount *account, QObject *parent) :
	AbstractConnection(account, parent), m_is_connected(false)
{
	m_infos << SNACInfo(ServiceFamily, ServerRedirectService)
			<< SNACInfo(AvatarFamily, AvatarGetReply);
	registerHandler(this);
	connect(socket(), SIGNAL(disconnected()), SLOT(disconnected()));
}

BuddyPicture::~BuddyPicture()
{
}

void BuddyPicture::sendUpdatePicture(QObject *reqObject, quint16 icon_id, quint8 icon_flags, const QByteArray &icon_hash)
{
	if (socket()->state() == QTcpSocket::UnconnectedState)
		return;
	QByteArray old_hash = reqObject->property("icon_hash").toByteArray();
	if (old_hash != icon_hash) {
		SNAC snac(AvatarFamily, AvatarGetRequest);
		snac.append<quint8>(reqObject->property("id").toString());
		snac.append<quint8>(1); // unknown
		snac.append<quint16>(icon_id);
		snac.append<quint8>(icon_flags);
		snac.append<quint8>(icon_hash);
		if (m_is_connected)
			send(snac);
		else
			m_history.push_back(snac);
	}
}

void BuddyPicture::handleSNAC(AbstractConnection *conn, const SNAC &snac)
{
	if (this == conn) {
		AbstractConnection::handleSNAC(this, snac);
		snac.resetState();
		if (snac.family() == ServiceFamily && snac.subtype() == ServiceServerAsksServices) {
			SNAC snac(ServiceFamily, ServiceClientReady);
			snac.append(QByteArray::fromHex(
					"0001 0004 0110 164f" // ServiceFamily
					"000f 0001 0110 164f"));// AvatarFamily
			send(snac);
			m_is_connected = true;
			while (!m_history.isEmpty()) {
				SNAC tmp = m_history.takeFirst();
				send(tmp);
			}
		}
	} else {
		if (snac.family() == ServiceFamily && snac.subtype() == ServerRedirectService) {
			TLVMap tlvs = snac.read<TLVMap>();
			quint16 id = tlvs.value(0x0D).read<quint16>();
			if (id == AvatarFamily) {
				QList<QByteArray> list = tlvs.value(0x05).data().split(':');
				m_cookie = tlvs.value(0x06).data();
				socket()->connectToHost(list.at(0), list.size() > 1 ? atoi(list.at(1).constData()) : 5190);
			}
		}
	}
	switch ((snac.family() << 16) | snac.subtype()) {
	case AvatarFamily << 16 | AvatarGetReply: {
		QString uin = snac.read<QString, quint8>();
		QObject *obj;
		if (uin == account()->id())
			obj = account();
		else
			obj = account()->getUnit(uin, false);
		if (!obj)
			break;
		snac.skipData(3); // skip icon_id and icon_flag
		QByteArray hash = snac.read<QByteArray, quint8>();
		snac.skipData(21);
		QByteArray image = snac.read<QByteArray, quint16>();
		if (!image.isEmpty()) {
			QString image_path = QString("%1/%2.%3/avatars/")
					.arg(SystemInfo::getPath(SystemInfo::ConfigDir))
					.arg(account()->protocol()->id())
					.arg(account()->id());
			QDir dir(image_path);
			if (!dir.exists())
				dir.mkpath(image_path);
			image_path += hash.toHex();
			QFile icon_file(image_path);
			if (!icon_file.exists()) {
				if (icon_file.open(QIODevice::WriteOnly))
					icon_file.write(image);
				obj->setProperty("icon_hash", hash);
			}
			obj->setProperty("avatar", image_path);
		}
		break;
	}
	}
}

void BuddyPicture::processNewConnection()
{
	FLAP flap(0x01);
	flap.append<quint32>(0x01);
	flap.appendTLV<QByteArray>(0x0006, m_cookie);
	send(flap);
}

void BuddyPicture::disconnected()
{
	m_is_connected = false;
	m_history.clear();
}

} } // namespace qutim_sdk_0_3::oscar

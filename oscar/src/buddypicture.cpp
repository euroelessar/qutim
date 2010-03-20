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
#include "sessiondataitem.h"
#include <QSet>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QNetworkProxy>

namespace qutim_sdk_0_3 {

namespace oscar {

QByteArray BuddyPicture::emptyHash = QByteArray::fromHex("0201d20472");

BuddyPicture::BuddyPicture(IcqAccount *account, QObject *parent) :
	AbstractConnection(account, parent), m_is_connected(false)
{
	m_infos << SNACInfo(ServiceFamily, ServerRedirectService)
			<< SNACInfo(AvatarFamily, AvatarGetReply);
	m_types << SsiBuddyIcon;
	registerHandler(this);
	socket()->setProxy(account->connection()->socket()->proxy());
	connect(socket(), SIGNAL(disconnected()), SLOT(disconnected()));
	account->feedbag()->registerHandler(this);
	account->registerRosterPlugin(this);

	typedef QPair<QString, QString> StringPair;
	ConfigGroup cfg = account->config("avatars").group("hashes");
	QList<StringPair > newList;
	foreach (const QString &id, cfg.groupList()) {
		IcqContact *contact = account->getContact(id);
		if (contact) {
			QString hash = cfg.value(id, QString());
			if (setAvatar(contact, QByteArray::fromHex(hash.toLatin1())))
				newList.push_back(StringPair(id, hash));
		}
	}
	cfg = cfg.parent();
	cfg.removeGroup("hashes");
	if (!newList.isEmpty()) {
		cfg = cfg.group("hashes");
		foreach (const StringPair &itr, newList)
			cfg.setValue(itr.first, itr.second);
	}
	cfg.sync();
}

BuddyPicture::~BuddyPicture()
{
}

void BuddyPicture::sendUpdatePicture(QObject *reqObject, quint16 id, quint8 flags, const QByteArray &hash)
{
	if (hash == emptyHash) {
		reqObject->setProperty("avatar", "");
		return;
	}
	if (setAvatar(reqObject, hash))
		return;
	if (socket()->state() == QTcpSocket::UnconnectedState)
		return;
	SNAC snac(AvatarFamily, AvatarGetRequest);
	snac.append<quint8>(reqObject->property("id").toString());
	snac.append<quint8>(1); // unknown
	snac.append<quint16>(id);
	snac.append<quint8>(flags);
	snac.append<quint8>(hash);
	if (m_is_connected)
		send(snac);
	else
		m_history.push_back(snac);
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
		snac.skipData(3); // skip iconId and iconFlag
		QByteArray hash = snac.read<QByteArray, quint8>();
		snac.skipData(21);
		QByteArray image = snac.read<QByteArray, quint16>();
		if (!image.isEmpty()) {
			QString imagePath = getAvatarDir();
			QDir dir(imagePath);
			if (!dir.exists())
				dir.mkpath(imagePath);
			imagePath += hash.toHex();
			QFile iconFile(imagePath);
			if (!iconFile.exists() && iconFile.open(QIODevice::WriteOnly)) {
				iconFile.write(image);
				updateData(obj, hash, imagePath);
			}
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
	m_cookie.clear();
	send(flap);
}

void BuddyPicture::processCloseConnection()
{
	m_is_connected = false;
}

bool BuddyPicture::handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error)
{
	Q_UNUSED(feedbag);
	Q_ASSERT(item.type() == SsiBuddyIcon);
	if (error != FeedbagError::NoError || type == Feedbag::Remove)
		return false;
	if (account()->avatarsSupport() && item.containsField(0x00d5)) {
		DataUnit data(item.field(0x00d5));
		quint8 flags = data.read<quint8>();
		QByteArray hash = data.read<QByteArray, quint8>();
		IcqContact *contact = account()->getContact(item.name());
		if (contact)
			sendUpdatePicture(contact, 1, flags, hash);
	}
	return true;
}

void BuddyPicture::statusChanged(IcqContact *contact, Status &status, const TLVMap &tlvs)
{
	Q_UNUSED(status);
	if (contact->status() == Status::Offline) {
		if (account()->avatarsSupport() && tlvs.contains(0x001d)) { // avatar
			SessionDataItemMap items(tlvs.value(0x001d));
			foreach (const SessionDataItem &item, items) {
				if (item.type() != staticAvatar && item.type() != miniAvatar &&
					item.type() != flashAvatar && item.type() != photoAvatar)
				{
					continue;
				}
				sendUpdatePicture(contact, item.type(), item.flags(), item.readData(16));
                break;
            }
		}
	}
}

void BuddyPicture::disconnected()
{
	m_is_connected = false;
	m_history.clear();
}

QString BuddyPicture::getAvatarDir() const
{
	return QString("%1/%2.%3/avatars/")
			.arg(SystemInfo::getPath(SystemInfo::ConfigDir))
			.arg(account()->protocol()->id())
			.arg(account()->id());
}

bool BuddyPicture::setAvatar(QObject *obj, const QByteArray &hash)
{
	if (obj->property("iconHash").toByteArray() == hash)
		return true;
	QFileInfo file(getAvatarDir() + hash.toHex());
	if (file.exists()) {
		updateData(obj, hash, file.filePath());
		return true;
	}
	return false;
}

void BuddyPicture::updateData(QObject *obj, const QByteArray &hash, const QString &path)
{
	obj->setProperty("iconHash", hash);
	obj->setProperty("avatar", path);
	ConfigGroup cfg = account()->config("avatars").group("hashes");
	cfg.setValue(obj->property("id").toString(), QString::fromLatin1(hash.toHex()));
	cfg.sync();
}

} } // namespace qutim_sdk_0_3::oscar

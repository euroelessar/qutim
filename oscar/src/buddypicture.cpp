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
#include "icqaccount_p.h"
#include "sessiondataitem.h"
#include <QSet>
#include <QDir>
#include <QFile>
#include <QImage>
#include <QNetworkProxy>
#include <QCryptographicHash>

namespace qutim_sdk_0_3 {

namespace oscar {

QByteArray BuddyPicture::emptyHash = QByteArray::fromHex("0201d20472");

BuddyPicture::BuddyPicture(IcqAccount *account, QObject *parent) :
	AbstractConnection(account, parent), m_avatars(false)
{
	updateSettings();
	m_infos << SNACInfo(ServiceFamily, ServerRedirectService)
			<< SNACInfo(ServiceFamily, ServiceServerExtstatus)
			<< SNACInfo(AvatarFamily, AvatarGetReply)
			<< SNACInfo(AvatarFamily, AvatarUploadAck);
	m_types << SsiBuddyIcon;
	registerHandler(this);
	socket()->setProxy(account->connection()->socket()->proxy());
	account->feedbag()->registerHandler(this);
	account->registerRosterPlugin(this);
	connect(account, SIGNAL(settingsUpdated()), this, SLOT(updateSettings()));

	typedef QPair<QString, QString> StringPair;
	Config cfg = account->config("avatars");
	Config hashesCfg = cfg.group("hashes");
	QList<StringPair > newList;
	foreach (const QString &id, hashesCfg.childKeys()) {
		IcqContact *contact = account->getContact(id);
		if (contact) {
			QString hash = hashesCfg.value(id, QString());
			if (setAvatar(contact, QByteArray::fromHex(hash.toLatin1())))
				newList.push_back(StringPair(id, hash));
		}
	}
	cfg.remove("hashes");
	if (!newList.isEmpty()) {
		cfg.beginGroup("hashes");
		foreach (const StringPair &itr, newList)
			cfg.setValue(itr.first, itr.second);
		cfg.endGroup();
	}
}

BuddyPicture::~BuddyPicture()
{
}

void BuddyPicture::sendUpdatePicture(QObject *reqObject, quint16 id, quint8 flags, const QByteArray &hash)
{
	if (setAvatar(reqObject, hash))
		return;
	if (!m_avatars)
		return;
	ensureConnection();
	SNAC snac(AvatarFamily, AvatarGetRequest);
	snac.append<quint8>(reqObject->property("id").toString());
	snac.append<quint8>(1); // unknown
	snac.append<quint16>(id);
	snac.append<quint8>(flags);
	snac.append<quint8>(hash);
	if (state() == Connected)
		send(snac);
	else
		m_history.insert(reqObject, snac);
}

void BuddyPicture::setAccountAvatar(const QString &avatar)
{
	m_accountAvatar.clear();
	QFile image(avatar);
	if (!image.open(QIODevice::ReadOnly))
		return;
	m_accountAvatar = image.read(8178); // TODO: notify user if the image size limit is exceeded
	// Md5 hash.
	m_avatarHash = QCryptographicHash::hash(m_accountAvatar, QCryptographicHash::Md5);
	// Request for update of avatar.
	FeedbagItem item = account()->feedbag()->type(SsiBuddyIcon, Feedbag::GenerateId).first();
	TLV data(0x00d5);
	data.append<quint8>(1);
	data.append<quint8>(m_avatarHash);
	item.setField(data);
	if (!item.isInList())
		item.setName("1");
	item.update();
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
			setState(Connected);
			foreach (SNAC snac, m_history)
				send(snac);
			m_history.clear();
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
		saveImage(obj, image, hash);
		break;
	}
	case ServiceFamily << 16 | ServiceServerExtstatus: { // account avatar changed
		TLVMap tlvs = snac.read<TLVMap>();
		if (tlvs.contains(0x0200)) {
			TLV tlv = tlvs.value(0x0200);
			quint8 type = tlv.read<quint8>();
			if (type == 0x0001) {
				quint8 flags = tlv.read<quint8>();
				QByteArray hash = tlv.read<QByteArray, quint8>();
				if (flags >> 6 & 0x1 && !m_accountAvatar.isEmpty()) { // does it really work???
					ensureConnection();
					SNAC snac(AvatarFamily, AvatarUploadRequest);
					snac.append<quint16>(1); // reference number ?
					snac.append<quint16>(m_accountAvatar);
					if (state() == Connected)
						send(snac);
					else
						m_history.insert(account(), snac);
				}
				setAvatar(account(), hash);
			}
		}
		break;
	}
	case AvatarFamily << 16 | AvatarUploadAck: { // avatar uploaded
		snac.skipData(4); // unknown
		QByteArray hash = snac.read<QByteArray, quint8>();
		if (hash == m_avatarHash) {
			saveImage(account(), m_accountAvatar, hash);
			debug() << "Account's avatar has been successfully updated";
		} else {
			debug() << "Error occurred when updating account avatar";
		}
		m_avatarHash.clear();
		m_accountAvatar.clear();
		break;
	}
	}
}

void BuddyPicture::processNewConnection()
{
	AbstractConnection::processNewConnection();
	FLAP flap(0x01);
	flap.append<quint32>(0x01);
	flap.appendTLV<QByteArray>(0x0006, m_cookie);
	m_cookie.clear();
	send(flap);
}

void BuddyPicture::processCloseConnection()
{
	AbstractConnection::processCloseConnection();
}

bool BuddyPicture::handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error)
{
	Q_UNUSED(feedbag);
	Q_ASSERT(item.type() == SsiBuddyIcon);
	if (error != FeedbagError::NoError || type == Feedbag::Remove)
		return false;
	if (m_avatars && m_accountAvatar.isEmpty() && item.containsField(0x00d5)) {
		DataUnit data(item.field(0x00d5));
		quint8 flags = data.read<quint8>();
		QByteArray hash = data.read<QByteArray, quint8>();
		if (m_avatarHash.isEmpty())
			sendUpdatePicture(account(), 1, flags, hash);
	}
	return true;
}

void BuddyPicture::statusChanged(IcqContact *contact, Status &status, const TLVMap &tlvs)
{
	Q_UNUSED(status);
	if (contact->status() == Status::Offline) {
		if (m_avatars && tlvs.contains(0x001d)) { // avatar
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

void BuddyPicture::onDisconnect()
{
	m_history.clear();
	m_avatarHash.clear();
	m_accountAvatar.clear();
	AbstractConnection::onDisconnect();
}

void BuddyPicture::updateSettings()
{
	m_avatars = account()->protocol()->config("general").value("avatars", true);
	if (m_avatars)
		account()->setProperty("rosterFlags", account()->property("rosterFlags").toInt() | 0x0005);
	else
		account()->setProperty("rosterFlags", account()->property("rosterFlags").toInt() ^ 0x0005);
}

QString BuddyPicture::getAvatarDir() const
{
	return QString("%1/avatars/%2/")
			.arg(SystemInfo::getPath(SystemInfo::ConfigDir))
			.arg(account()->protocol()->id());
}

bool BuddyPicture::setAvatar(QObject *obj, const QByteArray &hash)
{
	if (obj->property("iconHash").toByteArray() == hash)
		return true;
	if (hash == emptyHash) {
		updateData(obj, hash, "");
		return true;
	} else {
		QFileInfo file(getAvatarDir() + hash.toHex());
		if (file.exists()) {
			updateData(obj, hash, file.filePath());
			return true;
		}
	}
	return false;
}

void BuddyPicture::updateData(QObject *obj, const QByteArray &hash, const QString &path)
{
	obj->setProperty("iconHash", hash);
	if (IcqAccount *account = qobject_cast<IcqAccount*>(obj)) {
		account->d_func()->avatar = path;
		emit account->avatarChanged(path);
	} else {
		obj->setProperty("avatar", path);
	}
	Config cfg = account()->config("avatars").group("hashes");
	cfg.setValue(obj->property("id").toString(), QString::fromLatin1(hash.toHex()));
	cfg.sync();
}

void BuddyPicture::saveImage(QObject *obj, const QByteArray &image, const QByteArray &hash)
{
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
}

void BuddyPicture::ensureConnection()
{
	// Requesting avatar service
	SNAC snac(ServiceFamily, ServiceClientNewService);
	snac.append<quint16>(AvatarFamily);
	account()->connection()->send(snac);
}

} } // namespace qutim_sdk_0_3::oscar

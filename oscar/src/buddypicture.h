/****************************************************************************
 *  buddypicture.h
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

#ifndef BUDDYPICTURE_H_
#define BUDDYPICTURE_H_

#include "oscarconnection.h"
#include "icqcontact.h"
#include "snachandler.h"
#include "feedbag.h"
#include "roster.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqAccount;
class BuddyPicture;

enum IconItemType
{
	miniAvatar = 0x0000, // AIM mini avatar
	staticAvatar = 0x0001, // AIM/ICQ avatar ID/hash (len 5 or 16 bytes)
	iChatOnlineMessage = 0x0002, // iChat online message
	flashAvatar = 0x0008, // ICQ Flash avatar hash (16 bytes)
	itunesLink = 0x0009, // iTunes music store link
	photoAvatar = 0x000c, // ICQ contact photo (16 bytes)
	lastTime = 0x000D, // Last update time of online message
	statusMood = 0x000e // Status mood
};

class BuddyPicture: public AbstractConnection, public FeedbagItemHandler, public RosterPlugin
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::FeedbagItemHandler qutim_sdk_0_3::oscar::RosterPlugin)
public:
	BuddyPicture(IcqAccount *account, QObject *parent = 0);
	virtual ~BuddyPicture();
	void sendUpdatePicture(QObject *reqObject, quint16 icon_id, quint8 icon_flags, const QByteArray &icon_hash);
	void setAccountAvatar(const QString &avatar);
protected:
	void handleSNAC(AbstractConnection *conn, const SNAC &snac);
	void connectToServ(const QString &addr, quint16 port, const QByteArray &cookie);
	void processNewConnection();
	void processCloseConnection();
	virtual bool handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error);
	virtual void statusChanged(IcqContact *contact, Status &status, const TLVMap &tlvs);
	void onDisconnect();
private slots:
	void updateSettings();
private:
	inline QString getAvatarDir() const;
	inline bool setAvatar(QObject *obj, const QByteArray &hash);
	inline void updateData(QObject *obj, const QByteArray &hash, const QString &path);
	void saveImage(QObject *obj, const QByteArray &image, const QByteArray &hash);
private:
	QHash<QObject*, SNAC> m_history;
	bool m_is_connected;
	QByteArray m_cookie;
	bool m_avatars;
	QByteArray m_accountAvatar;
	QByteArray m_avatarHash;
	static QByteArray emptyHash;
};

} } // namespace qutim_sdk_0_3::oscar

#endif /* BUDDYPICTURE_H_ */

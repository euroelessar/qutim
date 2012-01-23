/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef BUDDYPICTURE_H_
#define BUDDYPICTURE_H_

#include "oscarconnection.h"
#include "icqcontact.h"
#include "snachandler.h"
#include "feedbag.h"
#include "oscarroster.h"

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
	bool m_startup;
	static QByteArray emptyHash;
};

} } // namespace qutim_sdk_0_3::oscar

#endif /* BUDDYPICTURE_H_ */


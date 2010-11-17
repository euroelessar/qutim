/****************************************************************************
 *  ircavatar.cpp
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

#include "ircavatar.h"
#include "ircaccount.h"
#include "irccontact.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QDir>
#include <QCryptographicHash>
#include <qutim/systeminfo.h>

Q_DECLARE_METATYPE(QPointer<qutim_sdk_0_3::irc::IrcContact>);

namespace qutim_sdk_0_3 {

namespace irc {

IrcAvatar *IrcAvatar::self = 0;

IrcAvatar::IrcAvatar(QObject *parent) :
	QObject(parent)
{
	m_ctpcCmds << "AVATAR";
}

void IrcAvatar::requestAvatar(IrcContact *contact)
{
	contact->account()->sendCtpcRequest(contact->id(), "AVATAR", QString());
}

void IrcAvatar::handleCtpcRequest(IrcAccount *account, const QString &sender, const QString &,
								  const QString &, const QString &cmd, const QString &)
{
	Q_ASSERT(cmd == "AVATAR");
	QString avatar = account->avatar();
	if (!avatar.isEmpty())
		account->sendCtpcReply(sender, "AVATAR", avatar);
}

void IrcAvatar::handleCtpcResponse(IrcAccount *account, const QString &sender, const QString &,
								   const QString &, const QString &cmd, const QString &params)
{
	Q_UNUSED(cmd);
	QString avatarUrlStr = params.section(" ", 0, 0);
	QUrl avatarUrl(avatarUrlStr);
	if (!avatarUrl.isValid())
		return;
	QPointer<IrcContact> contact = account->getContact(sender);
	if (!contact)
		return;
	QDir configDir = SystemInfo::getDir(SystemInfo::ConfigDir);
	QDir dir(configDir.path() + "/avatars/irc/");
	if (!dir.exists())
		configDir.mkpath("avatars/irc/");
	QByteArray avatarHash = QCryptographicHash::hash(avatarUrlStr.toUtf8(), QCryptographicHash::Md5).toHex();
	QString avatarPath = dir.path() + "/" + avatarHash;
	if (!QFileInfo(avatarPath).exists()) {
		QNetworkAccessManager *manager = new QNetworkAccessManager(this);
		connect(manager, SIGNAL(finished(QNetworkReply*)),
				this, SLOT(avatarReceived(QNetworkReply*)));
		QNetworkReply *reply = manager->get(QNetworkRequest(avatarUrl));
		reply->setProperty("avatarPath", avatarPath);
		reply->setProperty("contact", QVariant::fromValue(contact));
	} else {
		contact->setAvatar(avatarPath);
	}
}

void IrcAvatar::avatarReceived(QNetworkReply *reply)
{
	if (reply->rawHeader("Content-Length").toInt() < 256000) {
		QPointer<IrcContact> contact = reply->property("contact").value<QPointer<IrcContact> >();
		if (!contact)
			return;
		QString avatarPath = reply->property("avatarPath").toString();
		QFile file(avatarPath);
		if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			file.write(reply->readAll());
			file.close();
			contact->setAvatar(avatarPath);
		}
	}
}


QString IrcAvatar::getAvatarDir() const
{
	return QString("%1/avatars/irc/").arg(SystemInfo::getPath(SystemInfo::ConfigDir));
}

} } // namespace qutim_sdk_0_3::irc

/****************************************************************************
 *  icqcontact.h
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

#ifndef ICQCONTACT_H
#define ICQCONTACT_H

#include <qutim/contact.h>
#include <capability.h>

namespace Icq {

using namespace qutim_sdk_0_3;

class IcqAccount;
class Roster;
class DirectConnectionInfo;

struct IcqContactPrivate;

class IcqContact : public Contact
{
public:
	IcqContact(const QString &uin, IcqAccount *account);
	virtual QSet<QString> tags() const;
	virtual QString id() const;
	virtual QString name() const;
	virtual Status status() const;
	virtual void sendMessage(const Message &message);
	virtual void setName(const QString &name);
	virtual void setTags(const QSet<QString> &tags);
	virtual bool isInList() const;
	virtual void setInList(bool inList);
public:
	bool RtfSupport();
	bool HtmlSupport();
	bool TypingSupport();
	bool AimChatSupport();
	bool AimImageSupport();
	bool XtrazSupport();
	bool Utf8Support();
	bool SendFileSupport();
	bool DirectSupport();
	bool IconSupport();
	bool GetFileSupport();
	bool SrvRelaySupport();
	bool AvatarSupport();
	const Capabilities &capabilities();
	const Capabilities &shortCapabilities();
	const DirectConnectionInfo &dcInfo();
	void setStatus(Status status);
private:
	void clearCapabilities();
	friend class Roster;
	friend class MessagesHandler;
	QScopedPointer<IcqContactPrivate> p;
};

} // namespace Icq

#endif // ICQCONTACT_H

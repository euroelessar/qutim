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

#include <QScopedPointer>
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
	Q_DECLARE_PRIVATE(IcqContact)
	Q_OBJECT
	Q_DISABLE_COPY(IcqContact)
public:
	IcqContact(const QString &uin, IcqAccount *account);
	~IcqContact();
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
	bool RtfSupport() const;
	bool HtmlSupport() const;
	bool TypingSupport() const;
	bool AimChatSupport() const;
	bool AimImageSupport() const;
	bool XtrazSupport() const;
	bool Utf8Support() const;
	bool SendFileSupport() const;
	bool DirectSupport() const;
	bool IconSupport() const;
	bool GetFileSupport() const;
	bool SrvRelaySupport() const;
	bool AvatarSupport() const;
	const Capabilities &capabilities() const;
	const DirectConnectionInfo &dcInfo() const;
	void setStatus(Status status);
	void setCapabilities(const Capabilities &caps);
public slots:
	virtual void setChatState(ChatState state);
protected:
	friend class Roster;
	friend class MessagesHandler;
	QScopedPointer<IcqContactPrivate> d_ptr;
};

} // namespace Icq

#endif // ICQCONTACT_H

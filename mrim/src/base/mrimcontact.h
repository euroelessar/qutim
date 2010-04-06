#ifndef MRIMCONTACT_H
#define MRIMCONTACT_H

#include <QScopedPointer>
/****************************************************************************
 *  mrimcontact.h
 *
 *  Copyright (c) 2009 by Rusanov Peter <peter.rusanov@gmail.com>
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

#include <QDebug>

#include <qutim/contact.h>
#include "mrimconnection.h"
#include "useragent.h"
#include "proto.h"

using namespace qutim_sdk_0_3;

class MrimContact : public Contact
{
    Q_OBJECT
public:
    enum ContactFlag
    {
        ContactFlagRemoved	  =	0x00000001,
        ContactFlagGroup	  =	0x00000002,
        ContactFlagInvisible	  =	0x00000004,
        ContactFlagVisible	  =	0x00000008,
        ContactFlagIgnore	  =	0x00000010,
        ContactFlagShadow	  =	0x00000020,
        ContactFlagAuthorize	  =	0x00000040, // ( >= 1.15)
        ContactFlagPhone	  =	0x00100000,
        ContactFlagUnicodeName    =	0x00000200
    };

    Q_DECLARE_FLAGS(ContactFlags,ContactFlag)

public:
    MrimContact(class MrimAccount *account);
    virtual ~MrimContact();

    //from Contact
    QString id() const;
    void sendMessage(const Message &message);
    void setName(const QString &name);
    QSet<QString> tags() const;
    void setTags(const QSet<QString> &tags);
    bool isInList() const;
    void setInList(bool inList);
	void setChatState(ChatState state);
    //new
    quint32 contactId() const;
    void setContactId(quint32 id);
    quint32 groupId() const;
    void setGroupId(quint32 id);
    ContactFlags flags() const;
    void setFlags(ContactFlags flags);
    quint32 serverFlags() const;
    void setServerFlags(quint32 flags);
    QString email() const;
    void setEmail(const QString& email);
    MrimConnection::FeatureFlags featureFlags() const;
    void setFeatureFlags(MrimConnection::FeatureFlags flags);
    const UserAgent& userAgent() const;
    void setUserAgent(const UserAgent& agent);
    const MrimAccount *account() const;
    MrimAccount *account();

private:
    Q_DISABLE_COPY(MrimContact);
    QScopedPointer<struct MrimContactPrivate> p;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MrimContact::ContactFlags)

QDebug operator<<(QDebug dbg, const MrimContact &c);

#endif // MRIMCONTACT_H

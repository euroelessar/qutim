/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Rusanov Peter <peter.rusanov@gmail.com>
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

#ifndef MRIMCONTACT_H
#define MRIMCONTACT_H

#include <QScopedPointer>

#include <QDebug>

#include <qutim/contact.h>
#include "mrimconnection.h"
#include "mrimstatus.h"
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
    MrimContact(const QString &email, class MrimAccount *account);
    virtual ~MrimContact();

    //from Contact
    QString id() const;
	bool sendMessage(const Message &message);
    QString name() const;
    void setName(const QString &name);
    void setContactName(const QString &name);
	QStringList tags() const;
	void setTags(const QStringList &tags);
    bool isInList() const;
    void setInList(bool inList);
	void setContactInList(bool inList);
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
    const MrimUserAgent& userAgent() const;
    void setUserAgent(const MrimUserAgent& agent);
    const MrimAccount *account() const;
    MrimAccount *account();
    bool isPhone() const;
    Status status() const;
	MrimStatus mrimStatus() const;
    void setStatus(const MrimStatus &status);
	
	void clearComposingState();
	void updateComposingState();
	bool event(QEvent *);
	
protected:
	virtual void timerEvent(QTimerEvent *);
signals:
	void userAgentChanged(const MrimUserAgent &);
private:
    Q_DISABLE_COPY(MrimContact);
    QScopedPointer<struct MrimContactPrivate> p;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MrimContact::ContactFlags)

QDebug operator<<(QDebug dbg, const MrimContact &c);

#endif // MRIMCONTACT_H


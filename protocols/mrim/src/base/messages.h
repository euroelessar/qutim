/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef MESSAGES_H
#define MESSAGES_H

#include <QObject>
#include <QWeakPointer>
#include <qutim/message.h>

#include "mrimpacket.h"

using namespace qutim_sdk_0_3;

class MessagesPrivate;
class MrimContact;

class MrimMessages : public QObject, public PacketHandler
{
Q_OBJECT
public:
    enum Flag
    {
        MessageFlagNone           =   0x00000000,
        MessageFlagOffline        =   0x00000001,
        MessageFlagNoRecv         =   0x00000004,
        MessageFlagAuthorize      =   0x00000008,
        MessageFlagSystem         =   0x00000040,
        MessageFlagRtf            =   0x00000080,
        MessageFlagContact        =   0x00000200,
        MessageFlagTypingNotify   =   0x00000400,
        MessageFlagSMS            =   0x00000800,
        MessageFlagMulticast      =   0x00001000,
        MessageFlagSmsDelivery    =   0x00002000,
        MessageFlagAlarm          =   0x00004000,
        MessageFlagFlash          =   0x00008000,
        MessageFlagSpam           =   0x00020000,
        MessageFlagv1p16          =   0x00100000,
        MessageFlagCP1251         =   0x00200000
    };
    Q_DECLARE_FLAGS(Flags,Flag)

public:
    explicit MrimMessages(class MrimConnection *conn);
    virtual ~MrimMessages();
    virtual QList<quint32> handledTypes();
    virtual bool handlePacket(class MrimPacket& packet);
    quint32 sequence() const;
    void send(MrimContact *contact, const Message &msg, Flags flags = MessageFlagNone);
	void sendComposingNotification(MrimContact *contact);
    void sendDeliveryReport(const QString& from, quint32 msgId);

signals:

public slots:

protected:
    void send(MrimContact *contact, const QString &text, Flags flags, qint64 id = -1);

    virtual void handleMessageStatus(MrimPacket &packet);
    virtual void handleMessageAck(MrimPacket &packet);
    virtual void handleOfflineMessageAck(MrimPacket &packet);

private:
    QScopedPointer<MessagesPrivate> p;
};

#endif // MESSAGES_H


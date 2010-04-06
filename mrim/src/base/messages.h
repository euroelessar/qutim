#ifndef MESSAGES_H
#define MESSAGES_H

#include <QObject>
#include <QPointer>
#include <qutim/message.h>

#include "mrimpacket.h"

using namespace qutim_sdk_0_3;

class MessagesPrivate;

class Messages : public QObject, public PacketHandler
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
    explicit Messages(class MrimConnection *conn);
    virtual ~Messages();
    virtual QList<quint32> handledTypes();
    virtual bool handlePacket(class MrimPacket& packet);
    quint32 sequence() const;
    void send(const Message &msg, Flags flags = MessageFlagNone);

signals:

public slots:

protected:
    virtual void handleMessageStatus(MrimPacket &packet);

private:
    QScopedPointer<MessagesPrivate> p;
};

#endif // MESSAGES_H

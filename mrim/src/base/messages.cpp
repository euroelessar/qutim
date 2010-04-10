#include <QApplication>

#include <qutim/debug.h>
#include <qutim/messagesession.h>
#include <qutim/notificationslayer.h>

#include "messages.h"

#ifndef NO_RTF_SUPPORT
#include "rtfutils.h"
#endif

#include "proto.h"
#include "mrimdefs.h"
#include "mrimcontact.h"
#include "mrimaccount.h"

struct MsgIdLink
{
    quint64 msgId;
    QPointer<MrimContact> unit;
};

class MessagesPrivate
{
public:
    quint32 msgSeq;
    QPointer<MrimConnection> conn;
    QMap<quint32,MsgIdLink> msgIdLink;
#ifndef NO_RTF_SUPPORT
    Rtf *rtf;
#endif
};

Messages::Messages(MrimConnection *conn) :
    QObject(conn), p(new MessagesPrivate)
{
    p->msgSeq = 0;
    p->conn = conn;
    p->conn->registerPacketHandler(this);
#ifndef NO_RTF_SUPPORT
    p->rtf = new Rtf("cp1251");
#endif
}

Messages::~Messages()
{
#ifndef NO_RTF_SUPPORT
    delete p->rtf;
#endif
}

QList<quint32> Messages::handledTypes()
{
    return QList<quint32>() << MRIM_CS_MESSAGE_ACK
                            << MRIM_CS_MESSAGE_STATUS
                            << MRIM_CS_OFFLINE_MESSAGE_ACK
                            << MRIM_CS_AUTHORIZE_ACK;
}

bool Messages::handlePacket(class MrimPacket& packet)
{
    bool handled = true;

    switch (packet.msgType()) {
    case MRIM_CS_MESSAGE_STATUS:
        handleMessageStatus(packet);
        break;
    case MRIM_CS_MESSAGE_ACK:
        handleMessageAck(packet);
        break;
    default:
        handled = false;
        break;
    }
    return handled;
}

quint32 Messages::sequence() const
{
    return p->msgSeq;
}

void Messages::send(const Message &msg, Flags flags)
{
    const MrimContact *cnt = qobject_cast<const MrimContact*>(msg.chatUnit());

    if (cnt) {
        MrimPacket msgPacket(MrimPacket::Compose);
        msgPacket.setMsgType(MRIM_CS_MESSAGE);
        msgPacket.setSequence(sequence());

        while (p->msgIdLink.count() >= MRIM_MSGQUEUE_MAX_LEN) {
            p->msgIdLink.remove(p->msgIdLink.keys()[0]);
        }
        MsgIdLink link;
        link.msgId = msg.id();
        link.unit = const_cast<MrimContact*>(cnt);
        p->msgIdLink.insert(p->msgSeq++,link);

        msgPacket << flags;
        msgPacket.append(cnt->email());
        msgPacket.append(msg.text(),true);
        msgPacket.append(" ");
        p->conn->sendPacket(msgPacket);
    }
}

void Messages::handleMessageStatus(MrimPacket &packet)
{
    quint32 status = 0;
    packet.readTo(status);
    QString errString;
    MsgIdLink msgLink = p->msgIdLink[packet.sequence()];
    p->msgIdLink.remove(packet.sequence());
    ChatSession *sess = ChatLayer::instance()->getSession(msgLink.unit);
    bool delivered = false;

    switch (status) {
    case MESSAGE_DELIVERED:
        delivered = true;
        break;
    case MESSAGE_REJECTED_NOUSER:
        errString = tr("No such user");
        break;
    case MESSAGE_REJECTED_TOO_LARGE:
        errString = tr("Message too large");
        break;
    case MESSAGE_REJECTED_LIMIT_EXCEEDED:
        errString = tr("Limit exceeded");
        break;
    case MESSAGE_REJECTED_DENY_OFFMSG:
        errString = tr("User denied receiving offline messages");
        break;
    case MESSAGE_REJECTED_DENY_OFFFLSH:
        errString = tr("User denied receiving offline flash movies");
        break;
    default:
        errString = tr("Internal error");
        break;
    }

    if (sess) {
        QApplication::instance()->postEvent(sess, new MessageReceiptEvent(msgLink.msgId, delivered));
    }

    if (!errString.isEmpty()) {
        errString.prepend(tr("Message was not delivered!")+"\n");
        Notifications::sendNotification(Notifications::System,p->conn->account(),errString);
    }
}

void Messages::handleMessageAck(MrimPacket &packet)
{
    quint32 msgId = 0, flags = 0;
    packet.readTo(msgId);
    packet.readTo(flags);
    bool isAuth = (flags & MESSAGE_FLAG_AUTHORIZE);
    bool isUnicode = !(flags & MESSAGE_FLAG_CP1251);
#ifndef NO_RTF_SUPPORT
    bool hasRtf = (flags & MESSAGE_FLAG_RTF);
#endif
    bool isTyping = (flags & MESSAGE_FLAG_NOTIFY);

    if (isTyping) {
        //TODO: typing notifications
        return;
    }

    QString from,msg;
    packet.readTo(&from);
    packet.readTo(&msg,isUnicode);

    if (!isAuth && !(flags & MESSAGE_FLAG_NORECV)) {
        sendDeliveryReport(from,msgId);
    }

#ifndef NO_RTF_SUPPORT
    if (hasRtf) {
        QString rtfMsg;
        packet.readTo(&rtfMsg);
        msg = p->rtf->toPlainText(rtfMsg);
    }
#endif

    ChatLayer::instance()->getSession(p->conn->account(),from)->appendMessage(msg); //TODO: TEMP
}

void Messages::sendDeliveryReport(const QString& from, quint32 msgId)
{
    MrimPacket deliveryPacket(MrimPacket::Compose);
    deliveryPacket.setMsgType(MRIM_CS_MESSAGE_RECV);
    deliveryPacket << from;
    deliveryPacket << msgId;
    debug(Verbose)<<"Sending delivery report for msg #"<<msgId<<"...";
    p->conn->sendPacket(deliveryPacket);
}

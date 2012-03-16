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

#ifndef MESSAGES_P_H_
#define MESSAGES_P_H_

#include "messages.h"
#include <qutim/chatsession.h>

namespace qutim_sdk_0_3 {

namespace oscar {

class MessageSender;

class MessagesHandler : public QObject, public SNACHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::SNACHandler)
	Q_CLASSINFO("DependsOn", "qutim_sdk_0_3::oscar::IcqProtocol")
public:
	MessagesHandler();
	virtual ~MessagesHandler();
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
private slots:
	void loginFinished();
	void settingsUpdated();
	void accountAdded(qutim_sdk_0_3::Account *account);
private:
	void handleMessage(IcqAccount *account, const SNAC &snac);
	void handleResponse(IcqAccount *account, const SNAC &snac);
	QString handleChannel1Message(IcqContact *contact, const TLVMap &tlvs);
	QString handleChannel2Message(IcqContact *contact, const TLVMap &tlvs, quint64 msgCookie);
	QString handleChannel4Message(IcqContact *contact, const TLVMap &tlvs);
	QString handleTlv2711(const DataUnit &data, IcqContact *contact, quint16 ack, const Cookie &msgCookie);
	void sendMetaInfoRequest(IcqAccount *account, quint16 type);
	void sendChannel2Response(IcqContact *contact, quint8 type, quint8 flags, const Cookie &cookie);
private:
	QMultiHash<Capability, MessagePlugin *> m_msg_plugins;
	QMultiHash<Tlv2711Type, Tlv2711Plugin *> m_tlvs2711Plugins;
	bool m_detectCodec;
};

class MessageSender : public QObject
{
	Q_OBJECT
public:
	MessageSender(IcqAccount *account);
	bool appendMessage(IcqContact *contact, const Message &message);
private slots:
	void sendMessage();
	void messageTimeout(const Cookie &cookie);
private:
	IcqAccount *m_account;
	struct MessageData
	{
		MessageData(IcqContact *contact, const Message &message);
		IcqContact *contact;
		Message message;
		QList<QByteArray> msgs;
		quint8 channel;
		bool utfEnabled;
		quint64 id;
	};
	static void prepareMessage(IcqContact *contact, MessageData &data, const Message &message);
	void sendMessage(MessageData &message);
	QList<MessageData> m_messages;
	QTimer m_messagesTimer;
};

} } // namespace qutim_sdk_0_3::oscar

#endif /* MESSAGES_P_H_ */


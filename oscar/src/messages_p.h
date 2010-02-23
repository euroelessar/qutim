/****************************************************************************
 *  messages_p.h
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

#include "messages.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class MessagesHandler : public QObject, public SNACHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::SNACHandler)
public:
	MessagesHandler(IcqAccount *account, QObject *parent = 0);
	virtual ~MessagesHandler();
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
private slots:
	void loginFinished();
private:
	void handleMessage(const SNAC &snac);
	void handleResponse(const SNAC &snac);
	void handleChannel1Message(const SNAC &snac, IcqContact *contact, const QString &uin, const TLVMap &tlvs);
	void handleChannel2Message(const SNAC &snac, IcqContact *contact, const QString &uin, const TLVMap &tlvs, quint64 msgCookie);
	void handleChannel4Message(const SNAC &snac, IcqContact *contact, const QString &uin, const TLVMap &tlvs);
	void handleTlv2711(const DataUnit &data, IcqContact *contact, quint16 ack, const Cookie &msgCookie);
	void appendMessage(IcqContact *contact, const QString &message, QDateTime time = QDateTime());
	void sendChannel2Response(IcqContact *contact, quint8 type, quint8 flags, const Cookie &cookie);
	void sendMetaInfoRequest(quint16 type);
	IcqAccount *m_account;
	QMultiHash<Capability, MessagePlugin *> m_msg_plugins;
	QMultiHash<Tlv2711Type, Tlv2711Plugin *> m_tlvs2711Plugins;
};

} } // namespace qutim_sdk_0_3::oscar

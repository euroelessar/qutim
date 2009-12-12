/****************************************************************************
 *  clientidentify.h
 *
 *  Copyright (c) 2008 by Alexey Ignatiev <twosev@gmail.com>
 *  Copyright (c) 2009 by Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
****************************************************************************/


#ifndef CLIENTIDENTIFY_H_
#define CLIENTIDENTIFY_H_

#include <QList>
#include <QByteArray>
#include "capability.h"

namespace Icq {

class IcqContact;

class ClientIdentify
{
public:
	ClientIdentify();
	~ClientIdentify();
	void identify(IcqContact *contact);
private:
	void setClientData(const QString &clientId, const QString &icon);
	void setClientIcon(const QString &icon);
	QString icq_systemID2String(quint8 type, quint32 id);
	void identify_by_DCInfo();
	void parseMirandaVersionInDCInfo();
	void identify_by_ProtoVersion();
	void identify_qutIM();
	void identify_k8qutIM();
	void identify_Miranda();
	void identify_Qip();
	void identify_QipInfium();
	void identify_QipPDA();
	void identify_QipMobile();
	void identify_Sim();
	void identify_SimRnQ();
	void identify_Licq();
	void identify_Kopete();
	void identify_Micq();
	void identify_LibGaim();
	void identify_Jimm();
	void identify_Mip();
	void identify_Trillian();
	void identify_Climm();
	void identify_Im2();
	void identify_AndRQ();
	void identify_RandQ();
	void identify_Imadering();
	void identify_Mchat();
	void identify_CorePager();
	void identify_DiChat();
	void identify_Macicq();
	void identify_Anastasia();
	void identify_Jicq();
	void identify_Inlux();
	void identify_Vmicq();
	void identify_Smaper();
	void identify_Yapp();
	void identify_Pigeon();
	void identify_NatIcq();
	void identify_WebIcqPro();
	void identify_BayanIcq();
	void identify_MailAgent();
	void identify_Implux();
	void identify_IChat();
	void identify_StrIcq();
	void identify_NaimIcq();
private:
	IcqContact *m_contact;
	Capabilities m_client_caps;
	quint16 m_client_proto;
	quint32 m_info;
	quint32 m_ext_status_info;
	quint32 m_ext_info;
	QString m_client_id;
private:
	static const Capability ICQ_CAPABILITY_ICQJSINxVER;
	static const Capability ICQ_CAPABILITY_ICQJS7xVER;
	static const Capability ICQ_CAPABILITY_SIMxVER;
};

} // namespace Icq

#endif /*CLIENTIDENTIFY_H_*/

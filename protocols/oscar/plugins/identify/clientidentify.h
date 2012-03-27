/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2008 Alexey Ignatiev <twosev@gmail.com>
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef CLIENTIDENTIFY_H_
#define CLIENTIDENTIFY_H_

#include <QList>
#include <QByteArray>
#include "../../src/capability.h"
#include "../../src/oscarroster.h"
#include <qutim/plugin.h>

namespace qutim_sdk_0_3 {

class Account;
class Contact;

namespace oscar {

class IcqContact;

enum CapabilityFlag
{
	rtf_support       = 0x0001,
	html_support      = 0x0002,
	typing_support    = 0x0004,
	aim_chat_support  = 0x0008,
	aim_image_support = 0x0010,
	xtraz_support     = 0x0020,
	utf8_support      = 0x0040,
	sendfile_support  = 0x0080,
	direct_support    = 0x0100,
	icon_support      = 0x0200,
	getfile_support   = 0x0400,
	srvrelay_support  = 0x0800,
	avatar_support    = 0x1000
};

class ClientIdentify : public Plugin, public RosterPlugin
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::RosterPlugin)
	Q_CLASSINFO("DebugName", "ICQ/Identify")
	Q_CLASSINFO("DependsOn", "qutim_sdk_0_3::oscar::IcqProtocol")
public:
	Q_DECLARE_FLAGS(CapabilityFlags, CapabilityFlag)
	ClientIdentify();
	~ClientIdentify();
	void identify(IcqContact *contact);
	virtual void statusChanged(IcqContact *contact, Status &status, const TLVMap &tlvs);
	virtual void virtual_hook(int type, void *data);
	virtual void init();
	virtual bool load();
	virtual bool unload();
protected:
	bool eventFilter(QObject *obj, QEvent *ev);
private:
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
private:
	void setClientData(const QString &clientId, const QString &icon);
	void setClientIcon(const QString &icon);
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
	void identify_Jasmine();
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
	oscar::Capabilities m_client_caps;
	quint16 m_client_proto;
	quint32 m_info;
	quint32 m_ext_info;
	quint32 m_ext_status_info;
	QString m_client_id;
	ExtensionIcon m_client_icon;
	CapabilityFlags m_flags;
	QString m_client;

private:
	static const oscar::Capability ICQ_CAPABILITY_ICQJSINxVER;
	static const oscar::Capability ICQ_CAPABILITY_ICQJS7xVER;
	static const oscar::Capability ICQ_CAPABILITY_SIMxVER;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(ClientIdentify::CapabilityFlags);

inline bool ClientIdentify::RtfSupport() const
{
	return m_flags & rtf_support;
}

inline bool ClientIdentify::HtmlSupport() const
{
	return m_flags & html_support;
}

inline bool ClientIdentify::TypingSupport() const
{
	return m_flags & typing_support;
}

inline bool ClientIdentify::AimChatSupport() const
{
	return m_flags & aim_chat_support;
}

inline bool ClientIdentify::AimImageSupport() const
{
	return m_flags & aim_image_support;
}

inline bool ClientIdentify::XtrazSupport() const
{
	return m_flags & xtraz_support;
}

inline bool ClientIdentify::Utf8Support() const
{
	return m_flags & utf8_support;
}

inline bool ClientIdentify::SendFileSupport() const
{
	return m_flags & sendfile_support;
}

inline bool ClientIdentify::DirectSupport() const
{
	return m_flags & direct_support;
}

inline bool ClientIdentify::IconSupport() const
{
	return m_flags & icon_support;
}

inline bool ClientIdentify::GetFileSupport() const
{
	return m_flags & getfile_support;
}

inline bool ClientIdentify::SrvRelaySupport() const
{
	return m_flags & srvrelay_support;
}

inline bool ClientIdentify::AvatarSupport() const
{
	return m_flags & avatar_support;
}

} } // namespace qutim_sdk_0_3::oscar

#endif /*CLIENTIDENTIFY_H_*/


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

#include <QTextCodec>
#include <QtEndian>
#include <QStringBuilder>
#include <qutim/systeminfo.h>

#include "../../src/icqcontact.h"
#include "../../src/buddycaps.h"
#include "clientidentify.h"
#include "../../src/oscarconnection.h"
#include "../../src/xtraz.h"
#include "../../src/icqprotocol.h"
#include "../../src/icqaccount.h"
#include <qutim/tooltip.h>
#include <qutim/contact.h>

namespace qutim_sdk_0_3 {

namespace oscar {

void ClientIdentify::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "OscarIdentify"),
			QT_TRANSLATE_NOOP("Plugin", "Client identificaton for oscar contacts"),
			PLUGIN_VERSION(0, 0, 1, 0));
	setCapabilities(Loadable);
	addAuthor(QT_TRANSLATE_NOOP("Author", "Alexey Ignatiev"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("twosev@gmail.com"));
	addAuthor(QT_TRANSLATE_NOOP("Author", "Alexey Prokhin"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("alexey.prokhin@yandex.ru"));
	addExtension<ClientIdentify, RosterPlugin>(QT_TRANSLATE_NOOP("Plugin", "ICQ"),
				 QT_TRANSLATE_NOOP("Plugin", "Client identificaton for oscar contacts"),
				 ExtensionIcon("im-icq"));
}

bool ClientIdentify::load()
{
	Protocol *proto = Protocol::all().value("icq");
	if (!proto || proto->metaObject() != &IcqProtocol::staticMetaObject)
		return false;
	proto->installEventFilter(this);
	return true;
}

bool ClientIdentify::unload()
{
	return false;
}

bool ClientIdentify::eventFilter(QObject *obj, QEvent *ev)
{
	if (ev->type() == ExtendedInfosEvent::eventType() && obj == IcqProtocol::instance()) {
		ExtendedInfosEvent *event = static_cast<ExtendedInfosEvent*>(ev);
		QVariantHash clientInfo;
		clientInfo.insert("id", "client");
		clientInfo.insert("name", tr("Possible client"));
		clientInfo.insert("settingsDescription", tr("Show client icon"));
		event->addInfo("client", clientInfo);
	}
	return Plugin::eventFilter(obj, ev);
}

const oscar::Capability ClientIdentify::ICQ_CAPABILITY_ICQJSINxVER  ('s', 'i', 'n', 'j',  0x00, 0x00,
                                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                                     0x00, 0x00, 0x00, 0x00);

const oscar::Capability ClientIdentify::ICQ_CAPABILITY_ICQJS7xVER   ('i', 'c', 'q', 'j', 0x00, 0x00,
                                                                     0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                                     0x00, 0x00, 0x00, 0x00);

const oscar::Capability ClientIdentify::ICQ_CAPABILITY_SIMxVER      ('S',  'I',  'M',  ' ',  'c',  'l',
                                                                     'i',  'e',  'n',  't',  ' ',  ' ',
                                                                     0x00, 0x00, 0x00, 0x00);

#define IDENTIFY_CLIENT(func) \
	identify_##func(); \
	if(!m_client_id.isEmpty()) \
		return;

ClientIdentify::ClientIdentify()
{
}

ClientIdentify::~ClientIdentify()
{
}

void ClientIdentify::identify(IcqContact *contact)
{
	m_client_id.clear();
	m_contact = contact;
	m_client_caps = contact->capabilities();
	m_client_proto = contact->dcInfo().protocol_version;
	m_info = contact->dcInfo().info_utime;
	m_ext_info = contact->dcInfo().extinfo_utime;
	m_ext_status_info = contact->dcInfo().extstatus_utime;
	m_flags = 0;

	foreach(const oscar::Capability &capability, m_client_caps)
	{
		if (capability.match(ICQ_CAPABILITY_RTFxMSGS))
			m_flags |= rtf_support;
		else if (capability.match(ICQ_CAPABILITY_TYPING))
			m_flags |= typing_support;
		else if (capability.match(ICQ_CAPABILITY_AIMCHAT))
			m_flags |= aim_chat_support;
		else if (capability.match(ICQ_CAPABILITY_AIMIMAGE))
			m_flags |= aim_image_support;
		else if (capability.match(ICQ_CAPABILITY_XTRAZ))
			m_flags |= xtraz_support;
		else if (capability.match(ICQ_CAPABILITY_UTF8))
			m_flags |= utf8_support;
		else if (capability.match(ICQ_CAPABILITY_AIMSENDFILE))
			m_flags |= sendfile_support;
		else if (capability.match(ICQ_CAPABILITY_DIRECT))
			m_flags |= direct_support;
		else if (capability.match(ICQ_CAPABILITY_AIMICON))
			m_flags |= icon_support;
		else if (capability.match(ICQ_CAPABILITY_AIMGETFILE))
			m_flags |= getfile_support;
		else if (capability.match(ICQ_CAPABILITY_SRVxRELAY))
			m_flags |= srvrelay_support;
		else if (capability.match(ICQ_CAPABILITY_AVATAR))
			m_flags |= avatar_support;
	}

	// There may be some x-statuses info here.. remove all of them.
	// TODO:
	//Xtraz::removeXStatuses(m_client_caps);

	IDENTIFY_CLIENT( qutIM );
	IDENTIFY_CLIENT( k8qutIM );
	IDENTIFY_CLIENT( Miranda );
	IDENTIFY_CLIENT( Qip );
	IDENTIFY_CLIENT( QipInfium );
	IDENTIFY_CLIENT( QipPDA );
	IDENTIFY_CLIENT( QipMobile );
	IDENTIFY_CLIENT( SimRnQ );
	IDENTIFY_CLIENT( Sim );
	IDENTIFY_CLIENT( Licq );
	IDENTIFY_CLIENT( Kopete );
	IDENTIFY_CLIENT( Micq );
	IDENTIFY_CLIENT( LibGaim );
	IDENTIFY_CLIENT( Jimm );
	IDENTIFY_CLIENT( Mip );
	IDENTIFY_CLIENT( Jasmine );
	IDENTIFY_CLIENT( Trillian );
	IDENTIFY_CLIENT( Climm );
	IDENTIFY_CLIENT( Im2 );
	IDENTIFY_CLIENT( AndRQ );
	IDENTIFY_CLIENT( RandQ );
	IDENTIFY_CLIENT( Imadering );
	IDENTIFY_CLIENT( Mchat );
	IDENTIFY_CLIENT( CorePager );
	IDENTIFY_CLIENT( DiChat );
	IDENTIFY_CLIENT( Macicq );
	IDENTIFY_CLIENT( Anastasia );
	IDENTIFY_CLIENT( Jicq );
	IDENTIFY_CLIENT( Inlux );
	IDENTIFY_CLIENT( Vmicq );
	IDENTIFY_CLIENT( Smaper );
	IDENTIFY_CLIENT( Yapp );
	IDENTIFY_CLIENT( Pigeon );
	IDENTIFY_CLIENT( NatIcq );
	IDENTIFY_CLIENT( WebIcqPro );
	IDENTIFY_CLIENT( BayanIcq );
	IDENTIFY_CLIENT( by_ProtoVersion );
	IDENTIFY_CLIENT( by_DCInfo );
	setClientData("-", "unknown");
}

void ClientIdentify::statusChanged(IcqContact *contact, Status &status, const TLVMap &tlvs)
{
	Q_UNUSED(tlvs);
	if (status == Status::Offline) {
		status.removeExtendedInfo("client");
		return;
	}
	if (contact->status() == Status::Offline) {
		identify(contact);
		QVariantHash clientInfo;
		clientInfo.insert("id", "client");
		clientInfo.insert("title", tr("Possible client"));
		clientInfo.insert("icon", QVariant::fromValue(m_client_icon));
		clientInfo.insert("description", m_client_id);
		clientInfo.insert("showInTooltip", true);
		clientInfo.insert("priorityInContactList", 85);
		clientInfo.insert("priorityInTooltip", 25);
		clientInfo.insert("iconPosition", qVariantFromValue(ToolTipEvent::IconBeforeDescription));
		status.setExtendedInfo("client", clientInfo);
		debug() << contact->name() << "uses" << m_client_id;
	}
}

void ClientIdentify::virtual_hook(int type, void *data)
{
	Q_UNUSED(type);
	Q_UNUSED(data);
}

inline void ClientIdentify::setClientData(const QString &id, const QString &icon)
{
	m_client_id = id;
	setClientIcon(icon);
}

inline void ClientIdentify::setClientIcon(const QString &icon)
{
	m_client_icon = ExtensionIcon(icon + "-icq");
}

void ClientIdentify::identify_by_DCInfo()
{
	if ((m_info & 0xff000000) == 0x7d000000) {
		QString extra;
		unsigned long ver = m_info & 0xffff;
		extra = QString("%1.%2").arg(ver / 1000).arg((ver / 10) % 100);
		if(ver % 10)
			extra = QString("%1.%2").arg(extra).arg(ver % 10);
		m_client_id = "Licq v" + QString(extra);
		if ((m_info & 0x00ff0000) == 0x00800000)
			m_client_id += "/SSL";
		setClientIcon("licq");
	} else if (m_info == 0xffffffff) {
		if (m_ext_info == 0xffffffff)
			setClientData("Gaim", "gaim");
		else if ((!m_ext_info) && (m_client_proto == 7))
			setClientData("WebICQ", "webicq");
		else if ((!m_ext_info) && (m_ext_status_info == 0x3b7248ed))
			setClientData("Spam Bot", "icqbot");
		else
			parseMirandaVersionInDCInfo();
	} else if (m_info == 0x7fffffff) {
		parseMirandaVersionInDCInfo();
	} else if (m_info == 0x3b75ac09) {
		setClientData("Trillian", "trillian");
	} else if (m_info == 0xffffff42) {
		setClientData("mICQ", "micq");
	} else if (m_info == 0xffffff8f) {
		setClientData("StrICQ", "unknown");
	} else if (m_info == 0xffffff7f) {
		setClientData("&RQ", "RQ");
	} else if (m_info == 0xffffffab) {
		setClientData("YSM", "unknown");
	} else if (m_info == 0xffffffbe) {
		int ver1 = (m_ext_info >> 0x18) & 0xff;
		int ver2 = (m_ext_info >> 0x10) & 0xff;
		int ver3 = (m_ext_info >> 0x08) & 0xff;
		m_client_id = QString("Alicq %1.%2.%3").arg(ver1).arg(ver2).arg(ver3);
		setClientIcon("unknown");
	} else if (m_info == 0x04031980) {
		setClientData("vICQ", "unknown");
	} else if ((m_info == 0x3aa773ee) && (m_ext_info == 0x3aa66380)) {
		if (RtfSupport()) {
			setClientData("Centericq", "centerim-icq");
		} else {
			m_client_id = "libicq2000";
			if (Utf8Support())
				m_client_id += " (Unicode)";
			setClientIcon("icq-2000");
		}
	}
	else if ((m_info == 0x3ba8dbaf) && (m_client_proto == 2)) {
		setClientData("stICQ", "sticq");
	} else if ((m_info == 0xfffffffe) && (m_ext_status_info == 0xfffffffe)) {
		setClientData("Jimm", "jimm");
	} else if ((m_info == 0x3ff19beb) && (m_ext_status_info == 0x3ff19beb)) {
		setClientData("IM2", "im2");
	} else if ((m_info == 0xddddeeff) && (!m_ext_info) && (!m_ext_status_info)) {
		setClientData("SmartICQ", "unknown");
	} else if (((m_info & 0xfffffff0) == 0x494d2b00) && (!m_ext_info) && (!m_ext_status_info)) {
		setClientData("IM+", "implus");
	} else if ((m_info == 0x3b4c4c0c) && (!m_ext_info) && (m_ext_status_info == 0x3b7248ed)) {
		setClientData("KXicq2", "kxicq");
    } else if (m_info == 0xfffff666) {
		m_client_id = QString("R&Q %1").arg((unsigned int)m_ext_info);
		setClientIcon("rnq");
	} else if ((m_info == 0x66666666) && (m_ext_status_info == 0x66666666)) {
		m_client_id = "D[i]Chat";
		if (m_ext_info == 0x10000)
			m_client_id += " v.0.1a";
		else if (m_ext_info == 0x22)
			m_client_id += " v.0.2b";
		setClientIcon("di_chat");
	} else if ((m_info == 0x44F523B0) && (m_ext_info == 0x44F523A6) &&
			(m_ext_status_info == 0x44F523A6) && (m_client_proto == 8)) {
		setClientData("Virus", "unknown");
	}
}

void ClientIdentify::parseMirandaVersionInDCInfo()
{
	quint8 ver1 = (m_ext_info >> 0x18) & 0xFF;
	quint8 ver2 = (m_ext_info >> 0x10) & 0xFF;
	quint8 ver3 = (m_ext_info >> 0x08) & 0xFF;
	quint8 ver4 = m_ext_info & 0xFF;

	if(ver1 & 0x80)
		m_client_id = QString("Miranda IM (ICQ 0.%1.%2.%3 alpha)").arg(ver2).arg(ver3).arg(ver4);
	else
		m_client_id = QString("Miranda IM (ICQ %1.%2.%3.%4)").arg(ver1).arg(ver2).arg(ver3).arg(ver4);

	if (m_ext_status_info == 0x5AFEC0DE)
		m_client_id += " (SecureIM)";
	else if ((m_ext_status_info >> 0x18) == 0x80)
		m_client_id += " (Unicode)";
	setClientIcon("miranda");
}

void ClientIdentify::identify_by_ProtoVersion()
{
	static const oscar::Capability ICQ_CAPABILITY_IS2001      (0x2e, 0x7a, 0x64, 0x75, 0xfa, 0xdf,
	                                                           0x4d, 0xc8, 0x88, 0x6f, 0xea, 0x35,
	                                                           0x95, 0xfd, 0xb6, 0xdf);
	static const oscar::Capability ICQ_CAPABILITY_IS2002      (0x10, 0xcf, 0x40, 0xd1, 0x4c, 0x7f,
	                                                           0x11, 0xd1, 0x82, 0x22, 0x44, 0x45,
	                                                           0x53, 0x54, 0x00, 0x00);
	static const oscar::Capability ICQ_CAPABILITY_COMM20012   (0xa0, 0xe9, 0x3f, 0x37, 0x4c, 0x7f,
	                                                           0x11, 0xd1, 0x82, 0x22, 0x44, 0x45,
	                                                           0x53, 0x54, 0x00, 0x00);
	static const oscar::Capability ICQ_CAPABILITY_RAMBLER     (0x7E, 0x11, 0xB7, 0x78, 0xA3, 0x53,
	                                                           0x49, 0x26, 0xA8, 0x02, 0x44, 0x73,
	                                                           0x52, 0x08, 0xC4, 0x2A);
	static const oscar::Capability ICQ_CAPABILITY_ICQLITExVER (0x17, 0x8C, 0x2D, 0x9B, 0xDA, 0xA5,
	                                                           0x45, 0xBB, 0x8D, 0xDB, 0xF3, 0xBD,
	                                                           0xBD, 0x53, 0xA1, 0x0A);
	static const oscar::Capability ICQ_CAPABILITY_ABV         (0x00, 0xE7, 0xE0, 0xDF, 0xA9, 0xD0,
	                                                           0x4F, 0xe1, 0x91, 0x62, 0xC8, 0x90,
	                                                           0x9A, 0x13, 0x2A, 0x1B);
	static const oscar::Capability ICQ_CAPABILITY_NETVIGATOR  (0x4C, 0x6B, 0x90, 0xA3, 0x3D, 0x2D,
	                                                           0x48, 0x0E, 0x89, 0xD6, 0x2E, 0x4B,
	                                                           0x2C, 0x10, 0xD9, 0x9F);
	
	static const oscar::Capability ICQ_CAPABILITY_DIGSBY      (0x0002);
	
	static const oscar::Capability ICQ_CAPABILITY_ICQLITENEW  (0xC8, 0x95, 0x3A, 0x9F, 0x21, 0xF1,
	                                                           0x4f, 0xAA, 0xB0, 0xB2, 0x6D, 0xE6,
	                                                           0x63, 0xAB, 0xF5, 0xB7);

	// VERSION = 0
	if (m_client_proto == 0) {
		if (!m_info && !m_ext_info && !m_ext_status_info &&
			!(m_contact->dcInfo().port) && !(m_contact->dcInfo().auth_cookie))
		{
			if (TypingSupport() &&
				m_client_caps.match(ICQ_CAPABILITY_IS2001) &&
				m_client_caps.match(ICQ_CAPABILITY_IS2002) &&
				m_client_caps.match(ICQ_CAPABILITY_COMM20012))
			{
				setClientData("Spam Bot", "bot");
			}
			else if (IconSupport()) {
				if ((TypingSupport() && XtrazSupport() && SrvRelaySupport() && Utf8Support()) ||
					(Utf8Support() && (m_client_caps.size() == 2)) ||
					(m_client_caps.match(ICQ_CAPABILITY_ICQLITExVER) && XtrazSupport()))
				{
					setClientData("PyICQ-t Jabber Transport", "pyicq");
				}
				else if (Utf8Support() && SrvRelaySupport() && SendFileSupport() &&
						m_client_caps.match(ICQ_CAPABILITY_BUDDY_LIST) &&
						m_client_caps.match(ICQ_CAPABILITY_DIGSBY))
				{
					setClientData("Digsby", "digsby");
				}
			} else if (AimChatSupport()) {
				if (SendFileSupport() && (m_client_caps.size() == 2)) {
					setClientData("Easy Message", "unknown");
				} else if (IconSupport() && AimImageSupport()) {
					setClientData("AIM", "aim");
				}
			} else if (Utf8Support() && (m_client_caps.size() == 2)) {
				setClientData("BeejiveIM", "beejive");
			}
		} else if (Utf8Support() && SrvRelaySupport() && DirectSupport()) {
			if ((TypingSupport() && (m_client_caps.size() == 4)) ||
				(IconSupport() && AvatarSupport() && (m_client_caps.size() == 5)))
			{
				setClientData("Agile Messenger", "agile");
			} else if (SendFileSupport() && GetFileSupport()) {
				setClientData("Slick", "slick");
			}
		} else if(DirectSupport() && RtfSupport() && TypingSupport() && Utf8Support()) {
			setClientData("GlICQ", "glicq");
		}
	}

	// VERSION = 7
	else if (m_client_proto == 7) {
		if (RtfSupport()) {
			if (SrvRelaySupport() && DirectSupport() &&
				(m_info == 0x3aa773ee) &&
				(m_ext_info == 0x3aa66380) &&
				(m_ext_status_info == 0x3a877a42))
			{
				setClientData("centerim", "centerim");
			}
			else
			{
				setClientData("GnomeICU", "unknown");
			}
		} else if (SrvRelaySupport()) {
			if (!m_info && !m_ext_info && !m_ext_status_info) {
				setClientData("&RQ", "RQ");
			} else {
				setClientData("ICQ 2000", "icq-2000");
			}
		} else if (Utf8Support()) {
			if (TypingSupport())
				m_client_id = "Icq2Go! (Java)";
			else
				m_client_id = "Icq2Go! (Flash)";
			setClientIcon("icq-2go");
		}
	}

	// VERSION = 8
	else if (m_client_proto == 8) {
		if (XtrazSupport())
		{
			if (DirectSupport() && Utf8Support() && SrvRelaySupport() && AvatarSupport()) {
				setClientData("IM Gate", "imgate");
			} else if (m_client_caps.match(ICQ_CAPABILITY_IMSECKEY1) &&
					m_client_caps.match(ICQ_CAPABILITY_IMSECKEY2))
			{
				m_client_proto = 9;
				return identify_by_ProtoVersion();
			}
		}
		else if (m_client_caps.match(ICQ_CAPABILITY_COMM20012) || SrvRelaySupport()) {
			if (m_client_caps.match(ICQ_CAPABILITY_IS2001)) {
				if (!m_info && !m_ext_info && !m_ext_status_info) {
					if (RtfSupport()) {
						setClientData("TICQClient", "unknown"); // possibly also older GnomeICU
					} else {
						setClientData("ICQ for Pocket PC", "unknown");
					}
				} else {
					setClientData("ICQ 2001", "icq-2001");
				}
			} else if (m_client_caps.match(ICQ_CAPABILITY_IS2002)) {
				setClientData("ICQ 2002", "icq-2002");
			} else if (SrvRelaySupport() && Utf8Support() && RtfSupport() &&
					(!m_client_caps.match(ICQ_CAPABILITY_ICQJS7xVER)) &&
					(!m_client_caps.match(ICQ_CAPABILITY_ICQJSINxVER)))
			{
				if (!m_info && !m_ext_info && !m_ext_status_info) {
					if (!(m_contact->dcInfo().port)) {
						setClientData("GnomeICU 0.99.5+", "unknown");
					} else {
						setClientData("IC@", "unknown");
					}
				} else {
					setClientData("ICQ 2002/2003a", "icq-2002");
				}
			} else if (SrvRelaySupport() && Utf8Support() && TypingSupport()) {
				if (!m_info && !m_ext_info && !m_ext_status_info) {
					setClientData("PreludeICQ", "unknown");
				}
			}
		}
	}

	// VERSION = 9
	else if (m_client_proto == 9) {
		if (XtrazSupport()) {
			if (SendFileSupport()) {
				QString icon;
				if (m_client_caps.match(ICQ_CAPABILITY_ICQLITENEW)) {
					m_client_id = "ICQ 7";
					icon = "icq-70";
				} else if (m_client_caps.match(ICQ_CAPABILITY_TZERS)) {
					if (m_client_caps.match(ICQ_CAPABILITY_HTMLMSGS)) {
						if (RtfSupport()) {
							m_client_id = "MDC";
							icon = "mdc";
						} else {
							m_client_id = "ICQ 6";
							icon = "icq-60";
						}
					} else {
						m_client_id = "ICQ 5.1";
						icon = "icq-51";
					}
				} else {
					m_client_id = "ICQ 5";
					icon = "icq-50";
				}
				if (m_client_caps.match(ICQ_CAPABILITY_RAMBLER))
					m_client_id += " (Rambler)";
				else if (m_client_caps.match(ICQ_CAPABILITY_ABV))
					m_client_id += " (Abv)";
				else if (m_client_caps.match(ICQ_CAPABILITY_NETVIGATOR))
					m_client_id += " (Netvigator)";
				setClientIcon(icon);
			} else if (!DirectSupport()) {
				if (RtfSupport()) {
					m_client_id = "QNext";
					setClientIcon("unknown");
				}
				else if(m_client_caps.match(ICQ_CAPABILITY_TZERS)) {
					m_client_id = "Mail.Ru Agent";
					setClientIcon("mrim");
				}
				else {
					m_client_id = "PyICQ-t Jabber Transport";
					setClientIcon("pyicq");
				}
			} else {
				m_client_id = "ICQ Lite v4";
				setClientIcon("icq-4lite");
			}
		} else if(Utf8Support() && SendFileSupport() && IconSupport() && AimChatSupport()
				&& m_contact->capabilities().match(ICQ_CAPABILITY_BUDDY_LIST))
		{
			m_client_id = "ICQ Lite";
			setClientIcon("icq-4lite");
		}
		else if (!DirectSupport() && Utf8Support() && !RtfSupport()) {
			m_client_id = "PyICQ-t Jabber Transport";
			setClientIcon("pyicq");
		}
	}

	// VERSION = 10
	else if (m_client_proto == 0xA)
	{
		if (RtfSupport() && Utf8Support() && TypingSupport() && DirectSupport() && SrvRelaySupport())
			setClientData("ICQ 2003b Pro", "icq-2003pro");
		else if (!RtfSupport() && !Utf8Support())
			setClientData("QNext", "unknown");
		else if (!RtfSupport() && Utf8Support() && !m_info && !m_ext_info && !m_ext_status_info)
			setClientData("NanoICQ", "unknown");
	}
}

void ClientIdentify::identify_qutIM()
{
	static const oscar::Capability ICQ_CAPABILITY_QUTIMxVER ('q',  'u',  't',  'i',  'm',  0x00,
	                                                         0x00,  0x00,  0x00,  0x00,  0x00,
	                                                         0x00, 0x00, 0x00, 0x00, 0x00);

	oscar::Capabilities::const_iterator capit = m_client_caps.find(ICQ_CAPABILITY_QUTIMxVER);
	if (capit != m_client_caps.constEnd()) {
		const oscar::Capability &cap = *capit;
		QByteArray data = cap.data();
		const char *verStr = data.constData() + 5;
		if (verStr[1] == 46) {
			// old qutim id
			unsigned int ver1 = verStr[0] - 48;
			unsigned int ver2 = verStr[2] - 48;
			m_client_id = QString("qutIM v%1.%2").arg(ver1).arg(ver2);
		} else {
			// new qutim id
			quint8 type = verStr[0];
			quint32 ver = qFromBigEndian<quint32>((uchar *)verStr + 6);
			QString os = SystemInfo::systemID2String(type, ver);
			os.prepend('(');
			os.append (')');
			int ver1 = quint8(verStr[1]);
			int ver2 = quint8(verStr[2]);
			int ver3 = quint8(verStr[3]);
			int ver4 = quint8(verStr[4]);
			unsigned char svn1 = verStr[4];
			unsigned char svn2 = verStr[5];
			if (ver1 == 0x42) {
				m_client_id = QString("oldk8 v%1.%2 (%3) %4").arg(ver2).arg(ver3).
						           arg((unsigned int)((svn1 << 0x08))).arg(os);
			} else {
				if (svn2) {
					m_client_id = QString("qutIM v%1.%2.%3 svn%4 %5").arg(ver1).arg(ver2).arg(ver3).
							                  arg((unsigned int)((svn1 << 0x08) | svn2)).arg(os);
				} else {
					QString fixVersion;
					if (ver4 > 0) {
						fixVersion = QString::number(ver4);
						fixVersion.prepend(QLatin1Char('.'));
					}
					m_client_id = QString("qutIM v%1.%2.%3%4 %5")
					        .arg(QString::number(ver1), QString::number(ver2),
					             QString::number(ver3), fixVersion, os);
				}
			}
		}
		setClientIcon("qutim");
	}
}

void ClientIdentify::identify_k8qutIM()
{
	static const oscar::Capability ICQ_CAPABILITY_K8QUTIMxVER  ('k', '8', 'q', 'u', 't', 'I', 'M',
	                                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                            0x00, 0x00, 0x00);

	oscar::Capabilities::const_iterator cap = m_client_caps.find(ICQ_CAPABILITY_K8QUTIMxVER);
	if (cap != m_client_caps.constEnd()) {
		const char *cap_str = cap->data().data() + 7;
		QString os("");
		if(cap_str[0] != 'l')
			os = QString(" (%1)").arg(cap_str[0]);
		cap_str += 2;
		int ver0 = cap_str[0];
		int ver1 = cap_str[1];
		int ver2 = cap_str[2];
		unsigned char rel0 = cap_str[3];
		unsigned char rel1 = cap_str[4];
		m_client_id = QString("k8qutIM v%1.%2.%3.%4").arg(ver0).arg(ver1).arg(ver2).arg((unsigned int)((rel0 << 8) | rel1));
		m_client_id += os;
		setClientIcon("qutim-k8");
	}
}

void ClientIdentify::identify_Miranda()
{

	static const oscar::Capability ICQ_CAPABILITY_AIMOSCARxVER ('M', 'i', 'r', 'a', 'n', 'd', 'a',
	                                                            'A', 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                            0x00, 0x00, 0x00);
	
	static const oscar::Capability ICQ_CAPABILITY_ICQJPxVER    ('i', 'c', 'q', 'p', 0x00, 0x00,
	                                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                            0x00, 0x00, 0x00, 0x00);
	
	static const oscar::Capability ICQ_CAPABILITY_ICQJENxVER   ('e', 'n', 'q', 'j', 0x00, 0x00,
	                                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                            0x00, 0x00, 0x00, 0x00);
	
	static const oscar::Capability ICQ_CAPABILITY_MIRANDAxVER  ('M', 'i', 'r', 'a', 'n', 'd', 'a',
	                                                            'M', 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                            0x00, 0x00, 0x00);
	
	static const oscar::Capability ICQ_CAPABILITY_MIRMOBxVER   ('M', 'i', 'r', 'a', 'n', 'd', 'a',
	                                                            'M', 'o', 'b', 'i', 'l', 'e', 0x00,
	                                                            0x00, 0x00);
	
	static const oscar::Capability ICQ_CAPABILITY_ICQJS7SxVER  ('i', 'c', 'q', 'j', 0x00, 'S', 'e',
	                                                            'c', 'u', 'r', 'e', 0x00, 'I', 'M',
	                                                            0x00, 0x00);
	
	static const oscar::Capability ICQ_CAPABILITY_ICQJS7OxVER  (0x69, 0x63, 0x71, 0x6a, 0x00, 0x00,
	                                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                            0x00, 0x00, 0x00, 0x00);

	oscar::Capabilities::const_iterator cap;
	oscar::Capabilities::const_iterator end_itr = m_client_caps.constEnd();

	if (((cap = m_client_caps.find(ICQ_CAPABILITY_ICQJSINxVER)) != end_itr) ||
		((cap = m_client_caps.find(ICQ_CAPABILITY_ICQJS7xVER)) != end_itr) ||
		((cap = m_client_caps.find(ICQ_CAPABILITY_ICQJPxVER)) != end_itr) ||
		((cap = m_client_caps.find(ICQ_CAPABILITY_ICQJENxVER)) != end_itr))
	{
		const char *cap_str = cap->data().data();
		unsigned mver0   = cap_str[0x4] & 0xff;
		unsigned mver1   = cap_str[0x5] & 0xff;
		unsigned mver2   = cap_str[0x6] & 0xff;
		unsigned mver3   = cap_str[0x7] & 0xff;
		unsigned iver0   = cap_str[0x8] & 0xff;
		unsigned iver1   = cap_str[0x9] & 0xff;
		unsigned iver2   = cap_str[0xa] & 0xff;
		unsigned iver3   = cap_str[0xb] & 0xff;
		unsigned secure  = cap_str[0xc] & 0xff;
		unsigned ModVer  = cap_str[0x3] & 0xff;

		unsigned Unicode = (m_ext_status_info >> 24) & 0xff;
		if ((mver1 < 20) && (iver1 < 20)) {
			m_client_id +=  "Miranda IM ";
			if (mver0 == 0x80) {
				if (mver2 == 0x00)
					m_client_id += QString("0.%1 alpha build #%2").arg(mver1).arg(mver3);
				else
					m_client_id += QString("0.%1.%2 alpha build #%3").arg(mver1).arg(mver2).arg(mver3);
			} else {
				if (mver2 == 0x00)
					m_client_id += QString("%1.%2").arg(mver0).arg(mver1);
				else
					m_client_id += QString("%1.%2.%3").arg(mver0).arg(mver1).arg(mver2);
				if ((mver3 != 0x00) && (mver3 != 0x64))
					m_client_id += QString(" alpha build #%1").arg(mver3);
			}
			if ((Unicode == 0x80) || (m_info == 0x7fffffff))
				m_client_id += " Unicode";
			if (ModVer == 'p')
				m_client_id += " (ICQ Plus";
			else if (cap_str[0x0] == 's')
				m_client_id += " (ICQ S!N";
			else if (cap_str[0x0] == 'e')
				m_client_id += " (ICQ eternity/PlusPlus++";
			else if (ModVer == 'j')
				m_client_id += " (ICQ S7 & SSS";

			if (iver0 == 0x80)
				m_client_id += QString(" 0.%1.%2.%3 alpha)").arg(iver1).arg(iver2).arg(iver3);
			else
				m_client_id += QString(" %1.%2.%3.%4)").arg(iver0).arg(iver1).arg(iver2).arg(iver3);

			if (((secure != 0) && (secure != 20)) || (m_ext_status_info == 0x5AFEC0DE))
				m_client_id += " (SecureIM)";
			else if (m_client_caps.match(ICQ_CAPABILITY_ICQJS7SxVER, 0x10))
				m_client_id += "Miranda IM (ICQ SSS & S7)(SecureIM)";
			else if (m_client_caps.match(ICQ_CAPABILITY_ICQJS7OxVER, 0x10))
				m_client_id += "Miranda IM (ICQ SSS & S7)";
		}
	}
	else if ((cap = m_client_caps.find(ICQ_CAPABILITY_MIRANDAxVER)) != end_itr)
	{
		const char *cap_str = cap->data().data();
		unsigned mver0 = cap_str[0x8] & 0xff;
		unsigned mver1 = cap_str[0x9] & 0xff;
		unsigned mver2 = cap_str[0xA] & 0xff;
		unsigned mver3 = cap_str[0xB] & 0xff;
		unsigned iver0 = cap_str[0xC] & 0xff;
		unsigned iver1 = cap_str[0xD] & 0xff;
		unsigned iver2 = cap_str[0xE] & 0xff;
		unsigned iver3 = cap_str[0xF] & 0xff;
		m_client_id += "Miranda IM ";
		if (m_client_caps.match(ICQ_CAPABILITY_MIRMOBxVER))
			m_client_id += "Mobile ";
		if (mver0 == 0x80) {
			if (mver2 == 0x00)
				m_client_id += QString("0.%1 alpha build #%2").arg(mver1).arg(mver3);
			else
				m_client_id += QString("0.%1.%2 alpha build #%3").arg(mver1).arg(mver2).arg(mver3);
		} else {
			if (mver2 == 0x00)
				m_client_id += QString("%1.%2").arg(mver0).arg(mver1);
			else
				m_client_id += QString("%1.%2.%3").arg(mver0).arg(mver1).arg(mver2);
			if ((mver3 != 0x00) && (mver3 != 0x64))
				m_client_id += QString(" alpha build #%1").arg(mver3);
		}
		if ((m_info == 0x7fffffff) || ((unsigned)((m_ext_status_info >> 24) & 0xFF) == 0x80))
			m_client_id += " Unicode";
		m_client_id += " (ICQ ";
		if (m_client_caps.match(ICQ_CAPABILITY_ICQJS7OxVER, 0x10) || m_client_caps.match(ICQ_CAPABILITY_ICQJS7SxVER, 0x10))
			m_client_id += " S7 & SSS (old)";
		else
		{
			switch (iver0)
			{
			case 0x81:
				m_client_id += " BM";
			break;
			case 0x88:
				m_client_id += " eternity (old)";
			default:
			break;
			}
		}
		switch (iver2)
		{
		case 0x58:
			m_client_id += " eternity/PlusPlus++ Mod";
			break;
		default:
			break;
		}
		m_client_id += " ";
		switch (iver0)
		{
		case 0x88:
		case 0x81:
		case 0x80:
			m_client_id += QString("0.%1.%2.%3 alpha)").arg(iver1).arg(iver2).arg(iver3);
			break;
		default:
			m_client_id += QString("%1.%2.%3.%4)").arg(iver0).arg(iver1).arg(iver2).arg(iver3);
			break;
		}
		if ((m_ext_status_info == 0x5AFEC0DE) || m_client_caps.match(ICQ_CAPABILITY_ICQJS7SxVER, 0x10))
			m_client_id += " (SecureIM)";
	}
	if(!m_client_id.isEmpty())
		setClientIcon("miranda");
}

void ClientIdentify::identify_Qip()
{
	static const oscar::Capability ICQ_CAPABILITY_QIPxVER      (0x56, 0x3F, 0xC8, 0x09, 0x0B, 0x6F,
	                                                            0x41, 'Q', 'I', 'P', 0x00, 0x00, 0x00,
	                                                            0x00, 0x00, 0x00);

	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_QIPxVER)) != m_client_caps.constEnd()) {
		const QByteArray &cap_data = cap->data();
		m_client_id = "QIP ";
		if (m_ext_status_info == 0x0F)
			m_client_id += "2005";
		else
			m_client_id += QString::fromUtf8(cap_data.mid(11, 5));

		if ((m_ext_info == 0x0000000e) && (m_ext_status_info == 0x0000000f)) {
			m_client_id += QString(" (Build %1%2%3%4)")
					.arg((unsigned)m_info >> 0x18)
					.arg((unsigned)(m_info >> 0x10) & 0xFF)
					.arg((unsigned)(m_info >> 0x08) & 0xFF)
					.arg((unsigned)m_info & 0xFF);
		}
		setClientIcon("qip-2005");
	}
}

void ClientIdentify::identify_QipInfium()
{
	static const oscar::Capability ICQ_CAPABILITY_QIPINFxVER   (0x7c, 0x73, 0x75, 0x02, 0xc3, 0xbe,
	                                                            0x4f, 0x3e, 0xa6, 0x9f, 0x01, 0x53,
	                                                            0x13, 0x43, 0x1e, 0x1a);
	
	static const oscar::Capability ICQ_CAPABILITY_QIP2010xVER  (0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
	                                                            0x0a, 0x03, 0x0b, 0x04, 0x01, 0x53,
	                                                            0x13, 0x43, 0x1e, 0x1a);

	static const oscar::Capability ICQ_CAPABILITY_QIP2012xVER  (0x7f, 0x7f, 0x7c, 0x7d, 0x7e, 0x7f,
	                                                            0x0a, 0x03, 0x0b, 0x04, 0x01, 0x53,
	                                                            0x13, 0x43, 0x1e, 0x1a);

	bool qipInfium = m_client_caps.match(ICQ_CAPABILITY_QIPINFxVER);
	bool qip2010 = m_client_caps.match(ICQ_CAPABILITY_QIP2010xVER);
	bool qip2012 = m_client_caps.match(ICQ_CAPABILITY_QIP2012xVER);

	if (qip2012 || qip2010 || qipInfium) {
		QString icon = "qip";
		if (qipInfium) {
			m_client_id = "QIP Infium";
			icon += "-infium";
		} else if (qip2010) {
			m_client_id  = "QIP 2010";
			icon += "-2010";
		} else {
			m_client_id  = "QIP 2012";
			icon += "-2012";
		}
		if (m_info)
			m_client_id += QString(" (Build %1)").arg((unsigned)m_info);
		if (m_ext_info == 0x0000000B)
			m_client_id += " Beta";
		setClientIcon(icon);
	}
}

void ClientIdentify::identify_QipPDA()
{
	const oscar::Capability ICQ_CAPABILITY_QIPPDAxVER   (0x56, 0x3F, 0xC8, 0x09, 0x0B, 0x6F,
	                                                     0x41, 'Q', 'I', 'P', ' ', ' ', ' ',
	                                                     ' ', ' ', '!');
	
	if (m_client_caps.match(ICQ_CAPABILITY_QIPPDAxVER))
		setClientData("QIP PDA (Windows)", "qip-pda");
}

void ClientIdentify::identify_QipMobile()
{
	static const oscar::Capability ICQ_CAPABILITY_QIPMOBxVER   (0x56, 0x3F, 0xC8, 0x09, 0x0B, 0x6F,
	                                                            0x41, 'Q', 'I', 'P', ' ', ' ', ' ',
	                                                            ' ', ' ', '"');
	
	static const oscar::Capability ICQ_CAPABILITY_QIPSYMBxVER  (0x51, 0xad, 0xd1, 0x90, 0x72, 0x04,
	                                                            0x47, 0x3d, 0xa1, 0xa1, 0x49, 0xf4,
	                                                            0xa3, 0x97, 0xa4, 0x1f);

	if (m_client_caps.match(ICQ_CAPABILITY_QIPMOBxVER, 0x10))
		m_client_id = "QIP Mobile (Java)";
	else if (m_client_caps.match(ICQ_CAPABILITY_QIPSYMBxVER, 0x10))
		m_client_id = "QIP Mobile (Symbian)";
	if(!m_client_id.isEmpty())
		setClientIcon("qip-symbian");
}

void ClientIdentify::identify_Sim()
{
	/*static const Capability ICQ_CAPABILITY_SIMPLITE (0x53, 0x49, 0x4D, 0x50, 0x53, 0x49,
	                                                   0x4D, 0x50, 0x53, 0x49, 0x4D, 0x50,
	                                                   0x53, 0x49, 0x4D, 0x50);

	static const Capability ICQ_CAPABILITY_SIMPPRO    (0x53, 0x49, 0x4D, 0x50, 0x5F, 0x50,
	                                                   0x52, 0x4F, 0x53, 0x49, 0x4D, 0x50,
	                                                   0x5F, 0x50, 0x52, 0x4F);

	static const Capability ICQ_CAPABILITY_SIMOLDxVER (0x97, 0xb1, 0x27, 0x51, 0x24, 0x3c,
                                                       0x43, 0x34, 0xad, 0x22, 0xd6, 0xab,
                                                       0xf7, 0x3f, 0x14, 0x00);
    */
	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_SIMxVER)) != m_client_caps.constEnd()) {
		QString clientId = "SIM v";
		const char *cap_str = cap->data().data() + 12;
	    unsigned ver1 = cap_str[0];
	    unsigned ver2 = cap_str[1];
	    unsigned ver3 = cap_str[2];
	    unsigned ver4 = cap_str[3] & 0x0f;
		if (ver4)
			clientId += QString("%1.%2.%3.%4").arg(ver1).arg(ver2).arg(ver3).arg(ver4);
		else if (ver3)
			clientId += QString("%1.%2.%3").arg(ver1).arg(ver2).arg(ver3);
		else
			clientId += QString("%1.%2").arg(ver1).arg(ver2);
		if (cap_str[14] & 0x80)
			clientId += "/Win32";
		else if (cap_str[14] & 0x40)
			clientId += "/MacOS X";
		setClientIcon("sim");
	}
}

void ClientIdentify::identify_SimRnQ()
{
	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_SIMxVER)) != m_client_caps.constEnd()) {
		const char *cap_str = cap->data().data() + 12;
		if (cap_str[0] || cap_str[1] || cap_str[2] || (cap_str[3] & 0x0f))
			return;
	}
	else if(!m_client_caps.match(ICQ_CAPABILITY_SIMxVER, 10))
		return;
	setClientData("R&Q-masked (SIM)", "rnq");
}

void ClientIdentify::identify_Licq()
{
	static const oscar::Capability ICQ_CAPABILITY_LICQxVER  ('L',  'i',  'c',  'q',  ' ',  'c',
	                                                         'l',  'i',  'e',  'n',  't',  ' ',
	                                                         0x00, 0x00, 0x00, 0x00);

	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_LICQxVER)) != m_client_caps.constEnd()) {
		const char *cap_str = cap->data().data() + 12;
		unsigned ver1 = cap_str[0];
		unsigned ver2 = cap_str[1]%100;
		unsigned ver3 = cap_str[2];
		m_client_id = QString("Licq v%1.%2.%3").arg(ver1).arg(ver2).arg(ver3);
		if (cap_str[3] == 1)
			m_client_id += "/SSL";
		setClientIcon("licq");
	}
}

void ClientIdentify::identify_Kopete()
{
	static const oscar::Capability ICQ_CAPABILITY_KOPETExVER   ('K',  'o',  'p',  'e',  't',  'e',
	                                                            ' ',  'I',  'C',  'Q',  ' ',  ' ',
	                                                            0x00, 0x00, 0x00, 0x00);

	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_KOPETExVER)) != m_client_caps.constEnd()) {
		const char *cap_str = cap->data().data() + 12;
		unsigned ver1 = cap_str[0];
		unsigned ver2 = cap_str[1];
		unsigned ver3 = cap_str[2]*100;
		ver3 += cap_str[3];
		m_client_id = QString("Kopete v%1.%2.%3").arg(ver1).arg(ver2).arg(ver3);
		setClientIcon("kopete");
	}
}

void ClientIdentify::identify_Micq()
{
	static const oscar::Capability ICQ_CAPABILITY_MICQxVER ('m',  'I',  'C',  'Q',  ' ', 0xA9,
	                                                        ' ',  'R',  '.',  'K',  '.',  ' ',
	                                                        0x00, 0x00, 0x00, 0x00);

	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_MICQxVER)) != m_client_caps.constEnd()) {
		m_client_id = "mICQ v";
		const char *cap_str = cap->data().data() + 12;
		unsigned ver1 = cap_str[0];
		unsigned ver2 = cap_str[1];
		unsigned ver3 = cap_str[2];
		unsigned ver4 = cap_str[3];
		m_client_id += QString("%1.%2.%3.%4").arg(ver1).arg(ver2).arg(ver3).arg(ver4);
		if ((ver1 & 0x80) == 0x80)
			m_client_id += " alpha";
		setClientIcon("micq");
	}
}

void ClientIdentify::identify_LibGaim()
{
	int newver = 0;
	if (AimChatSupport()) {
		if (TypingSupport())
			newver = XtrazSupport() && SrvRelaySupport() ? 5 : 2;
		else
			newver = 1;
	}

	if (SendFileSupport() && AimImageSupport() &&
		IconSupport() && Utf8Support() &&
		(m_client_caps.size() <= (4 + newver)))
	{
		if (newver >= 1)
			setClientData("Pidgin/AdiumX", "pidgin");
		else
			setClientData("Gaim/AdiumX", "gaim");
	} else if ((newver >= 1) && Utf8Support()) {
		if((m_client_proto == 0 && IconSupport()) ||
		   (m_client_proto == 8 && m_client_caps.contains(ICQ_CAPABILITY_BART)))
		{
			setClientData("Meebo", "meebo");
		}
	}
}

void ClientIdentify::identify_Jimm()
{
	static const oscar::Capability ICQ_CAPABILITY_JIMMxVER ('J', 'i', 'm', 'm', ' ', 0x00,
	                                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                        0x00, 0x00, 0x00, 0x00);

	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_JIMMxVER)) != m_client_caps.constEnd()) {
		m_client_id = "Jimm ";
		m_client_id += QString::fromUtf8(cap->data().mid(5, 11));
		setClientIcon("jimm");
	}
}

void ClientIdentify::identify_Mip()
{
	static const oscar::Capability ICQ_CAPABILITY_MIPCLIENT (0x4D, 0x49, 0x50, 0x20, 0x00, 0x00,
	                                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                         0x00, 0x00, 0x00, 0x00);

	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_MIPCLIENT, 12)) != m_client_caps.constEnd()) {
		m_client_id = "MIP ";
		const char *cap_str = cap->data().data() + 12;
	    unsigned ver1 = cap_str[0];

	    if (ver1 < 30) {
	    	unsigned ver2 = cap_str[1];
			unsigned ver3 = cap_str[2];
			unsigned ver4 = cap_str[3];
	    	m_client_id = QString("%1.%2.%3.%4").arg(ver1).arg(ver2).arg(ver3).arg(ver4);
	    }
	    else
	    	m_client_id += QString::fromUtf8(cap->data().mid(11, 5));
	} else if ((cap = m_client_caps.find(ICQ_CAPABILITY_MIPCLIENT)) != m_client_caps.constEnd()) {
		m_client_id = "MIP ";
		m_client_id += QString::fromUtf8(cap->data().mid(4, 12));
	}
	if(!m_client_id.isEmpty())
		setClientIcon("mip");
}

void ClientIdentify::identify_Jasmine()
{
	static const oscar::Capability ICQ_CAPABILITY_JASMINExID  (0x4a, 0x61, 0x73, 0x6d, 0x69, 0x6e,
	                                                           0x65, 0x20, 0x49, 0x43, 0x51, 0x20,
	                                                           0x23, 0x23, 0x23, 0x23 );
	static const oscar::Capability ICQ_CAPABILITY_JASMINExVER (0x4a, 0x61, 0x73, 0x6d, 0x69, 0x6e,
	                                                           0x65, 0x20, 0x76, 0x65, 0x72, 0xff,
	                                                           0x00, 0x00, 0x00, 0x00 );
	if (m_client_caps.match(ICQ_CAPABILITY_JASMINExID)) {
		oscar::Capabilities::const_iterator cap = m_client_caps.find(ICQ_CAPABILITY_JASMINExVER);
		if (cap != m_client_caps.constEnd()) {
			QByteArray data = cap->data();
			const char *cap_str = data.constData() + 12;
			m_client_id = QLatin1String("Jasmine ");
			m_client_id += QString(QLatin1String("%1.%2.%3"))
			               .arg(QString::number(quint8(cap_str[0])),
			                    QString::number(quint8(cap_str[1])),
			                    QString::number(quint8(cap_str[2])));
			setClientIcon(QLatin1String("jasmine"));
		}
	}
}

void ClientIdentify::identify_Trillian()
{
	static const oscar::Capability ICQ_CAPABILITY_TRILLIANxVER (0x97, 0xb1, 0x27, 0x51, 0x24, 0x3c,
	                                                            0x43, 0x34, 0xad, 0x22, 0xd6, 0xab,
	                                                            0xf7, 0x3f, 0x14, 0x09 );
	
	static const oscar::Capability ICQ_CAPABILITY_TRILCRPTxVER (0xf2, 0xe7, 0xc7, 0xf4, 0xfe, 0xad,
	                                                            0x4d, 0xfb, 0xb2, 0x35, 0x36, 0x79,
	                                                            0x8b, 0xdf, 0x00, 0x00 );

	if (m_client_caps.match(ICQ_CAPABILITY_TRILLIANxVER) ||
		m_client_caps.match(ICQ_CAPABILITY_TRILCRPTxVER))
	{
		m_client_id = "Trillian";
		if (RtfSupport()) {
			if (SendFileSupport())
				m_client_id += " Astra";
			else
				m_client_id += " v3";
		}
		setClientIcon("trillian");
	}
}

void ClientIdentify::identify_Climm()
{
	static const oscar::Capability ICQ_CAPABILITY_CLIMMxVER ('c', 'l', 'i', 'm', 'm', 0xA9, ' ',
	                                                         'R', '.', 'K', '.', ' ', 0x00,
	                                                         0x00, 0x00, 0x00);

	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_CLIMMxVER)) != m_client_caps.constEnd()) {
		const char *cap_str = cap->data().data() + 12;
	    unsigned ver1 = cap_str[0];
	    unsigned ver2 = cap_str[1];
		unsigned ver3 = cap_str[2];
		unsigned ver4 = cap_str[3];

		m_client_id = QString("climm %1.%2.%3.%4").arg(ver1).arg(ver2).arg(ver3).arg(ver4);
		if ((ver1 & 0x80) == 0x80)
			m_client_id += " alpha";
		if (m_ext_status_info == 0x02000020)
			m_client_id += "/Win32";
		else if (m_ext_status_info == 0x03000800)
			m_client_id += "/MacOS X";
		setClientIcon("climm"); // ???
	}
}

void ClientIdentify::identify_Im2()
{
	static const oscar::Capability ICQ_CAPABILITY_IM2xVER (0x74, 0xED, 0xC3, 0x36, 0x44, 0xDF,
	                                                       0x48, 0x5B, 0x8B, 0x1C, 0x67, 0x1A,
	                                                       0x1F, 0x86, 0x09, 0x9F);

	if (m_client_caps.match(ICQ_CAPABILITY_IM2xVER))
		setClientData("IM2", "im2");
}

void ClientIdentify::identify_AndRQ()
{
	const oscar::Capability ICQ_CAPABILITY_ANDRQxVER    ('&', 'R', 'Q', 'i', 'n', 's', 'i',
	                                                     'd', 'e', 0x00, 0x00, 0x00, 0x00,
	                                                     0x00, 0x00, 0x00);

	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_ANDRQxVER)) != m_client_caps.constEnd()) {
		const char *cap_str = cap->data().data();
		unsigned ver1 = cap_str[0xC];
		unsigned ver2 = cap_str[0xB];
		unsigned ver3 = cap_str[0xA];
		unsigned ver4 = cap_str[9];
		m_client_id = QString("&RQ %1.%2.%3.%4").arg(ver1).arg(ver2).arg(ver3).arg(ver4);
		setClientIcon("rq");
	}
}

void ClientIdentify::identify_RandQ()
{
	static const oscar::Capability ICQ_CAPABILITY_RANDQxVER ('R', '&', 'Q', 'i', 'n', 's', 'i',
	                                                         'd', 'e', 0x00, 0x00, 0x00, 0x00,
	                                                         0x00, 0x00, 0x00);

	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_RANDQxVER)) != m_client_caps.constEnd()) {
		const char *cap_str = cap->data().data();
		unsigned ver1 = cap_str[0xC];
		unsigned ver2 = cap_str[0xB];
		unsigned ver3 = cap_str[0xA];
		unsigned ver4 = cap_str[9];
		m_client_id = QString("&RQ %1.%2.%3.%4").arg(ver1).arg(ver2).arg(ver3).arg(ver4);
		setClientIcon("rnq");
	}
}

void ClientIdentify::identify_Imadering()
{
	static const oscar::Capability ICQ_CAPABILITY_IMADERING  ('I', 'M', 'a', 'd', 'e', 'r', 'i',
	                                                          'n', 'g', ' ', 'C', 'l', 'i', 'e',
	                                                          'n', 't');

	if (m_client_caps.match(ICQ_CAPABILITY_IMADERING, 0x10))
		setClientData("IMadering", "unknown"); // icon ???
}

void ClientIdentify::identify_Mchat()
{

	const oscar::Capability ICQ_CAPABILITY_MCHATxVER    ('m', 'C', 'h', 'a', 't', ' ', 'i',
	                                                     'c', 'q', ' ', 0x00, 0x00, 0x00,
	                                                     0x00, 0x00, 0x00);
	
	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_MCHATxVER)) != m_client_caps.constEnd()) {
		m_client_id = "mChat ";
		m_client_id += QString::fromUtf8(cap->data().mid(10, 6));
		setClientIcon("mchat");
	}
}

void ClientIdentify::identify_CorePager()
{
	static const oscar::Capability ICQ_CAPABILITY_COREPGRxVER ('C', 'O', 'R', 'E', ' ', 'P', 'a',
	                                                           'g', 'e', 'r', 0x00, 0x00, 0x00,
	                                                           0x00, 0x00, 0x00);
	
	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_COREPGRxVER)) != m_client_caps.constEnd()) {
		m_client_id += "CORE Pager";
		if ((m_ext_info == 0x0FFFF0011) &&
		        (m_ext_status_info == 0x1100FFFF) &&
		        (m_info >> 0x18))
		{
			m_client_id += QString(" %1.%2").arg((unsigned)m_info >> 0x18).arg((unsigned)(m_info >> 0x10) & 0xFF);
			if ((m_info & 0xFF) == 0x0B)
				m_client_id += " Beta";
		}
		setClientIcon("jimm-corepager");
    }
}

void ClientIdentify::identify_DiChat()
{
	static const oscar::Capability ICQ_CAPABILITY_DICHATxVER ('D', '[', 'i', ']', 'C', 'h', 'a',
	                                                          't', ' ', 0x00, 0x00, 0x00, 0x00,
	                                                          0x00, 0x00, 0x00);
	
	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_DICHATxVER)) != m_client_caps.constEnd()) {
		m_client_id += "D[i]Chat ";
		m_client_id += QString::fromUtf8(cap->data().mid(8, 8));
		setClientIcon("dichat");
	}
}

void ClientIdentify::identify_Macicq()
{
	static const oscar::Capability ICQ_CAPABILITY_MACICQxVER (0xdd, 0x16, 0xf2, 0x02, 0x84, 0xe6,
	                                                          0x11, 0xd4, 0x90, 0xdb, 0x00, 0x10,
	                                                          0x4b, 0x9b, 0x4b, 0x7d);
	
	if (m_client_caps.match(ICQ_CAPABILITY_MACICQxVER, 0x10))
		setClientData("ICQ for Mac", "icq-mac");
}

void ClientIdentify::identify_Anastasia()
{
	static const oscar::Capability ICQ_CAPABILITY_ANSTxVER (0x44, 0xE5, 0xBF, 0xCE, 0xB0, 0x96,
	                                                        0xE5, 0x47, 0xBD, 0x65, 0xEF, 0xD6,
	                                                        0xA3, 0x7E, 0x36, 0x02);
	
	if (m_client_caps.match(ICQ_CAPABILITY_ANSTxVER))
		setClientData("Anastasia", "anastasia");
}

void ClientIdentify::identify_Jicq()
{
	
	static const oscar::Capability ICQ_CAPABILITY_PALMJICQ ('J', 'I', 'C', 'Q', 0x00, 0x00,
	                                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                        0x00, 0x00, 0x00, 0x00);
	
	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_PALMJICQ, 0xc)) != m_client_caps.constEnd()) {
		const char *cap_str = cap->data().data();
		unsigned ver1 = cap_str[0xC];
		unsigned ver2 = cap_str[0xD];
		unsigned ver3 = cap_str[0xE];
		unsigned ver4 = cap_str[0xF];
		m_client_id = QString("JICQ %1.%2.%3.%4").arg(ver1).arg(ver2).arg(ver3).arg(ver4);
		setClientIcon("jicq");
	}
}

void ClientIdentify::identify_Inlux()
{
	static const oscar::Capability ICQ_CAPABILITY_INLUXMSGR (0xA7, 0xE4, 0x0A, 0x96, 0xB3, 0xA0,
	                                                         0x47, 0x9A, 0xB8, 0x45, 0xC9, 0xE4,
	                                                         0x67, 0xC5, 0x6B, 0x1F);
	
	if (m_client_caps.match(ICQ_CAPABILITY_INLUXMSGR, 0x10))
		setClientData("Inlux Messenger", "inlux");
}

void ClientIdentify::identify_Vmicq()
{
	static const oscar::Capability ICQ_CAPABILITY_VMICQxVER (0x56, 0x6d, 0x49, 0x43, 0x51, 0x00,
	                                                         0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                         0x00, 0x00, 0x00, 0x00);
	
	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_VMICQxVER)) != m_client_caps.constEnd()) {
		m_client_id += "VmICQ ";
		m_client_id += QString::fromUtf8(cap->data().mid(5, 11));
		setClientIcon("vmicq");
	}
}

void ClientIdentify::identify_Smaper()
{
	static const oscar::Capability ICQ_CAPABILITY_SMAPERxVER ('S', 'm', 'a', 'p', 'e', 'r', ' ',
	                                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                          0x00, 0x00, 0x00);
	
	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_SMAPERxVER)) != m_client_caps.constEnd()) {
		m_client_id += "SmapeR ";
		m_client_id += QString::fromUtf8(cap->data().mid(6, 10));
		setClientIcon("smaper");
	}
}

void ClientIdentify::identify_Yapp()
{
	static const oscar::Capability ICQ_CAPABILITY_YAPPxVER (0x59, 0x61, 0x70, 0x70, 0x00, 0x00,
	                                                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                        0x00, 0x00, 0x00, 0x00);
	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_YAPPxVER)) != m_client_caps.constEnd()) {
		m_client_id = "Yapp! v";
		m_client_id += QString::fromUtf8(cap->data().mid(8, 5));
		setClientIcon("yapp");
	}
}

void ClientIdentify::identify_Pigeon()
{
	static const oscar::Capability ICQ_CAPABILITY_PIGEONxVER ('P', 'I', 'G', 'E', 'O', 'N', '!',
	                                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                          0x00, 0x00, 0x00);
	
	if (m_client_caps.match(ICQ_CAPABILITY_PIGEONxVER))
		setClientData("Pigeon", "pigeon");
}

void ClientIdentify::identify_NatIcq()
{
	static const oscar::Capability ICQ_CAPABILITY_NATICQxVER ('N', 'a', 't', 'I', 'C', 'Q',
	                                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                          0x00, 0x00, 0x00, 0x00);
	
	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_NATICQxVER)) != m_client_caps.constEnd()) {
		m_client_id = QString("NatICQ Siemens (revision %s)").arg(QString::fromUtf8(cap->data().mid(0xc, 4)));
		setClientIcon("naticq");
	}
}

void ClientIdentify::identify_WebIcqPro()
{
	static const oscar::Capability ICQ_CAPABILITY_WEBICQPRO ('W', 'e', 'b', 'I', 'c', 'q', 'P',
	                                                         'r', 'o', 0x00, 0x00, 0x00, 0x00,
	                                                         0x00, 0x00, 0x00);
	
	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_WEBICQPRO)) != m_client_caps.constEnd()) {
		const char *cap_str = cap->data().data();
	    unsigned ver1 = cap_str[0xA];
	    unsigned ver2 = cap_str[0xB];
	    unsigned ver3 = cap_str[0xC];
	    m_client_id = QString("WebIcqPro %1.%2.%3").arg(ver1).arg(ver2).arg(ver3);
	    if (cap_str[0xF] == 'b')
	    	m_client_id += "b";
	    setClientIcon("webicq");
	}
}

void ClientIdentify::identify_BayanIcq()
{
	static const oscar::Capability ICQ_CAPABILITY_BAYANICQxVER ('b', 'a', 'y', 'a', 'n', 'I', 'C',
	                                                            'Q', 0x00, 0x00, 0x00, 0x00, 0x00,
	                                                            0x00, 0x00, 0x00);
	
	oscar::Capabilities::const_iterator cap;
	if ((cap = m_client_caps.find(ICQ_CAPABILITY_BAYANICQxVER)) != m_client_caps.constEnd()) {
		m_client_id = "bayanICQ v";
		m_client_id += QString::fromUtf8(cap->data().mid(8, 8));
		setClientIcon("bayanicq");
	}
}

void ClientIdentify::identify_MailAgent()
{
//	char *capID;
//	char[] ICQ_CAPABILITY_MAILAGENT = {}
}

void ClientIdentify::identify_Implux()
{
	/*static const Capability ICQ_CAPABILITY_IMPLUXxVER (0x8e, 0xcd, 0x90, 0xe7, 0x4f, 0x18,
	                                                   0x28, 0xf8, 0x02, 0xec, 0xd6, 0x18,
	                                                   0xa4, 0xe9, 0xde, 0x68);*/
}

void ClientIdentify::identify_IChat()
{
	/*static const Capability ICQ_CAPABILITY_ICHAT (0x09, 0x46, 0x00, 0x00, 0x4C, 0x7F,
	                                              0x11, 0xD1, 0x82, 0x22, 0x44, 0x45,
	                                              0x53, 0x54, 0x00, 0x00);*/
}

void ClientIdentify::identify_StrIcq()
{
	/*static const Capability ICQ_CAPABILITY_STRICQxVER (0xa0, 0xe9, 0x3f, 0x37, 0x4f, 0xe9,
	                                                   0xd3, 0x11, 0xbc, 0xd2, 0x00, 0x04,
	                                                   0xac, 0x96, 0xdd, 0x96); */
}

void ClientIdentify::identify_NaimIcq()
{
	/*static const Capability ICQ_CAPABILITY_NAIMxVER (0xFF, 0xFF, 0xFF, 0xFF, 'n', 'a',
												     'i', 'm', 0x00, 0x00, 0x00, 0x00,
												     0x00, 0x00, 0x00, 0x00);*/
}

} } // namespace qutim_sdk_0_3::oscar

QUTIM_EXPORT_PLUGIN(qutim_sdk_0_3::oscar::ClientIdentify);


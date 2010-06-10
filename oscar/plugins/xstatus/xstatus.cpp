/****************************************************************************
 *  xstatus.cpp
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

#include "xstatus.h"
#include "icqcontact.h"
#include "icqaccount.h"
#include "oscarconnection.h"
#include "sessiondataitem.h"
#include "qutim/icon.h"
#include <qutim/extensionicon.h>
#include <qutim/statusactiongenerator.h>
#include <qutim/message.h>

namespace qutim_sdk_0_3 {

namespace oscar {

QHash<Capability, OscarStatus> XStatusHandler::qipstatuses;

static void init_xstatus_list(XStatusList &list)
{
	list << XStatus()
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Angry"), "angry", 23,
		       Capability(0x01, 0xD8, 0xD7, 0xEE, 0xAC, 0x3B, 0x49, 0x2A,
				  0xA5, 0x8D, 0xD3, 0xD8, 0x77, 0xE6, 0x6B, 0x92))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Taking a bath"), "bath", 1,
		       Capability(0x5A, 0x58, 0x1E, 0xA1, 0xE5, 0x80, 0x43, 0x0C,
				  0xA0, 0x6F, 0x61, 0x22, 0x98, 0xB7, 0xE4, 0xC7))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Tired"), "tired", 2,
		       Capability(0x83, 0xC9, 0xB7, 0x8E, 0x77, 0xE7, 0x43, 0x78,
				  0xB2, 0xC5, 0xFB, 0x6C, 0xFC, 0xC3, 0x5B, 0xEC))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Party"), "party", 3,
		       Capability(0xE6, 0x01, 0xE4, 0x1C, 0x33, 0x73, 0x4B, 0xD1,
				  0xBC, 0x06, 0x81, 0x1D, 0x6C, 0x32, 0x3D, 0x81))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Drinking beer"), "beer", 4,
		       Capability(0x8C, 0x50, 0xDB, 0xAE, 0x81, 0xED, 0x47, 0x86,
				  0xAC, 0xCA, 0x16, 0xCC, 0x32, 0x13, 0xC7, 0xB7))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Thinking"), "thinking", 5,
		       Capability(0x3F, 0xB0, 0xBD, 0x36, 0xAF, 0x3B, 0x4A, 0x60,
				  0x9E, 0xEF, 0xCF, 0x19, 0x0F, 0x6A, 0x5A, 0x7F))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Eating"), "eating", 6,
		       Capability(0xF8, 0xE8, 0xD7, 0xB2, 0x82, 0xC4, 0x41, 0x42,
				  0x90, 0xF8, 0x10, 0xC6, 0xCE, 0x0A, 0x89, 0xA6))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Watching TV"), "tv", 7,
		       Capability(0x80, 0x53, 0x7D, 0xE2, 0xA4, 0x67, 0x4A, 0x76,
				  0xB3, 0x54, 0x6D, 0xFD, 0x07, 0x5F, 0x5E, 0xC6))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Meeting"), "meeting", 8,
		       Capability(0xF1, 0x8A, 0xB5, 0x2E, 0xDC, 0x57, 0x49, 0x1D,
				  0x99, 0xDC, 0x64, 0x44, 0x50, 0x24, 0x57, 0xAF))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Coffee"), "coffee", 9,
		       Capability(0x1B, 0x78, 0xAE, 0x31, 0xFA, 0x0B, 0x4D, 0x38,
				  0x93, 0xD1, 0x99, 0x7E, 0xEE, 0xAF, 0xB2, 0x18))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Listening to music"), "music", 10,
		       Capability(0x61, 0xBE, 0xE0, 0xDD, 0x8B, 0xDD, 0x47, 0x5D,
				  0x8D, 0xEE, 0x5F, 0x4B, 0xAA, 0xCF, 0x19, 0xA7))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Business"), "business", 11,
		       Capability(0x48, 0x8E, 0x14, 0x89, 0x8A, 0xCA, 0x4A, 0x08,
				  0x82, 0xAA, 0x77, 0xCE, 0x7A, 0x16, 0x52, 0x08))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Shooting"), "shooting", 12,
		       Capability(0x10, 0x7A, 0x9A, 0x18, 0x12, 0x32, 0x4D, 0xA4,
				  0xB6, 0xCD, 0x08, 0x79, 0xDB, 0x78, 0x0F, 0x09))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Having fun"), "fun", 13,
		       Capability(0x6F, 0x49, 0x30, 0x98, 0x4F, 0x7C, 0x4A, 0xFF,
				  0xA2, 0x76, 0x34, 0xA0, 0x3B, 0xCE, 0xAE, 0xA7))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "On the phone"), "phone", 14,
		       Capability(0x12, 0x92, 0xE5, 0x50, 0x1B, 0x64, 0x4F, 0x66,
				  0xB2, 0x06, 0xB2, 0x9A, 0xF3, 0x78, 0xE4, 0x8D))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Gaming"), "gaming", 15,
		       Capability(0xD4, 0xA6, 0x11, 0xD0, 0x8F, 0x01, 0x4E, 0xC0,
				  0x92, 0x23, 0xC5, 0xB6, 0xBE, 0xC6, 0xCC, 0xF0))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Studying"), "studying", 16,
		       Capability(0x60, 0x9D, 0x52, 0xF8, 0xA2, 0x9A, 0x49, 0xA6,
				  0xB2, 0xA0, 0x25, 0x24, 0xC5, 0xE9, 0xD2, 0x60))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Shopping"), "shopping", 0,
		       Capability(0x63, 0x62, 0x73, 0x37, 0xA0, 0x3F, 0x49, 0xFF,
				  0x80, 0xE5, 0xF7, 0x09, 0xCD, 0xE0, 0xA4, 0xEE))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Feeling sick"), "sick", 17,
		       Capability(0x1F, 0x7A, 0x40, 0x71, 0xBF, 0x3B, 0x4E, 0x60,
				  0xBC, 0x32, 0x4C, 0x57, 0x87, 0xB0, 0x4C, 0xF1))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Sleeping"), "sleeping", 18,
		       Capability(0x78, 0x5E, 0x8C, 0x48, 0x40, 0xD3, 0x4C, 0x65,
				  0x88, 0x6F, 0x04, 0xCF, 0x3F, 0x3F, 0x43, 0xDF))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Surfing"), "surfing", 19,
		       Capability(0xA6, 0xED, 0x55, 0x7E, 0x6B, 0xF7, 0x44, 0xD4,
				  0xA5, 0xD4, 0xD2, 0xE7, 0xD9, 0x5C, 0xE8, 0x1F))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Browsing"), "browsing", 20,
		       Capability(0x12, 0xD0, 0x7E, 0x3E, 0xF8, 0x85, 0x48, 0x9E,
				  0x8E, 0x97, 0xA7, 0x2A, 0x65, 0x51, 0xE5, 0x8D))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Working"), "working", 21,
		       Capability(0xBA, 0x74, 0xDB, 0x3E, 0x9E, 0x24, 0x43, 0x4B,
				  0x87, 0xB6, 0x2F, 0x6B, 0x8D, 0xFE, 0xE5, 0x0F))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Typing"), "typing", 22,
		       Capability(0x63, 0x4F, 0x6B, 0xD8, 0xAD, 0xD2, 0x4A, 0xA1,
				  0xAA, 0xB9, 0x11, 0x5B, 0xC2, 0x6D, 0x05, 0xA1))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Picnic"), "picnic", -1,
		       Capability(0x2C, 0xE0, 0xE4, 0xE5, 0x7C, 0x64, 0x43, 0x70,
				  0x9C, 0x3A, 0x7A, 0x1C, 0xE8, 0x78, 0xA7, 0xDC))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Cooking"), "cooking", -1,
		       Capability(0x10, 0x11, 0x17, 0xC9, 0xA3, 0xB0, 0x40, 0xF9,
				  0x81, 0xAC, 0x49, 0xE1, 0x59, 0xFB, 0xD5, 0xD4))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Smoking"), "smoking", -1,
		       Capability(0x16, 0x0C, 0x60, 0xBB, 0xDD, 0x44, 0x43, 0xF3,
				  0x91, 0x40, 0x05, 0x0F, 0x00, 0xE6, 0xC0, 0x09))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "I'm high"), "high", -1,
		       Capability(0x64, 0x43, 0xC6, 0xAF, 0x22, 0x60, 0x45, 0x17,
				  0xB5, 0x8C, 0xD7, 0xDF, 0x8E, 0x29, 0x03, 0x52))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "On WC"), "wc", -1,
		       Capability(0x16, 0xF5, 0xB7, 0x6F, 0xA9, 0xD2, 0x40, 0x35,
				  0x8C, 0xC5, 0xC0, 0x84, 0x70, 0x3C, 0x98, 0xFA))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "To be or not to be"), "hamlet", -1,
		       Capability(0x63, 0x14, 0x36, 0xff, 0x3f, 0x8a, 0x40, 0xd0,
				  0xa5, 0xcb, 0x7b, 0x66, 0xe0, 0x51, 0xb3, 0x64))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Watching pro7 on TV"), "pro7", -1,
		       Capability(0xb7, 0x08, 0x67, 0xf5, 0x38, 0x25, 0x43, 0x27,
				  0xa1, 0xff, 0xcf, 0x4c, 0xc1, 0x93, 0x97, 0x97))
	    << XStatus(QT_TRANSLATE_NOOP("XStatus", "Love"), "love", -1,
		       Capability(0xdd, 0xcf, 0x0e, 0xa9, 0x71, 0x95, 0x40, 0x48,
				  0xa9, 0xc6, 0x41, 0x32, 0x06, 0xd6, 0xf2, 0x80));
}
Q_GLOBAL_STATIC_WITH_INITIALIZER(XStatusList, xstatusList, init_xstatus_list(*x));

QipExtendedStatus::QipExtendedStatus(quint16 status, const QString &iconName, const LocalizedString &name, quint16 id):
	OscarStatus(status)
{
	Capability cap(0xb7074378, 0xf50c7777, 0x97775778, (quint32)0x502d << 16 | id);
	setIcon(Icon(iconName));
	setName(name);
	setCapability(cap, "qipstatus");
	XStatusHandler::qipstatuses.insert(cap, *this);
}

XStatus::XStatus(const LocalizedString &status_, const QString &icon_,
				 qint8 mood_, const Capability &capability_) :
	status(status_), icon(QLatin1String("user-xstatus-") + icon_),
	mood(mood_), capability(capability_)
{
}

void XStatusHandler::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "OscarXStatus"),
			QT_TRANSLATE_NOOP("Plugin", "Additional statuses for oscar protocol"),
			PLUGIN_VERSION(0, 0, 1, 0));
	addAuthor(QT_TRANSLATE_NOOP("Author", "Alexey Prokhin"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("alexey.prokhin@yandex.ru"));
	addExtension<XStatusHandler, Tlv2711Plugin, RosterPlugin>
		(QT_TRANSLATE_NOOP("Plugin", "ICQ"),
		 QT_TRANSLATE_NOOP("Plugin", "Additional statuses for oscar protocol"),
		 ExtensionIcon("im-icq"));
}

bool XStatusHandler::load()
{
	{
		Capability cap(0xb7074378, 0xf50c7777, 0x97775778, 0x502d0575);
		OscarStatus status(Status::FreeChat);
		status.setSubtype(OscarOnline);
		status.setCapability(cap, "qipstatus");
		qipstatuses.insert(cap, status);
		OscarStatus::registerStatus(OscarFFC, status);
	}
	QipExtendedStatus(OscarOnline, "user-online-angry-icq",
					  QT_TRANSLATE_NOOP("Status", "Angry"), 0x0579);
	QipExtendedStatus(OscarOnline, "user-online-depression-icq",
					  QT_TRANSLATE_NOOP("Status", "Depression"), 0x0570);
	QipExtendedStatus(OscarOnline, "user-online-athome-icq",
					  QT_TRANSLATE_NOOP("Status", "At home"), 0x0576);
	QipExtendedStatus(OscarOnline, "user-online-atwork-icq",
					  QT_TRANSLATE_NOOP("Status", "At work"), 0x0577);
	QipExtendedStatus(OscarAway, "user-away-eating-icq",
					  QT_TRANSLATE_NOOP("Status", "Eating"), 0x0578);
	foreach (Status status, qipstatuses)
		MenuController::addAction(new StatusActionGenerator(status), &IcqAccount::staticMetaObject);
	return true;
}

bool XStatusHandler::unload()
{
	return false;
}

XStatusHandler::XStatusHandler()
{
	m_tlvs2711Types << Tlv2711Type(MSG_XSTRAZ_SCRIPT, xtrazNotify);
}

void XStatusHandler::processTlvs2711(IcqContact *contact, Capability guid, quint16 type, const DataUnit &data, const Cookie &cookie)
{
	QString message = data.read<QString, quint32>(LittleEndian);
	Q_ASSERT(guid == MSG_XSTRAZ_SCRIPT && type == xtrazNotify);
	Xtraz xtraz(message);
	if (xtraz.type() == Xtraz::Request) {
		XtrazRequest request = xtraz.request();
		if (request.pluginId() != "srvMng" && request.serviceId() != "cAwaySrv" &&
			request.value("senderId") != contact->id())
		{
			debug() << "Skipped xtraz request" << request.serviceId() << "from" << request.value("senderId");
			return;
		}
		IcqAccount *account = contact->account();
		QVariantHash extStatuses = account->status().property("extendedStatuses", QVariantHash());
		QVariantHash extStatus = extStatuses.value("xstatus", QVariantHash()).value<QVariantHash>();
		XtrazResponse response("cAwaySrv", "OnRemoteNotification");
		response.setValue("CASXtraSetAwayMessage", "");
		response.setValue("uin", account->id());
		response.setValue("index", extStatus.value("index").toString());
		response.setValue("title", extStatus.value("title").toString());
		response.setValue("desc", extStatus.value("desc").toString());
		SNAC snac = response.snac(contact, cookie.id());
		account->connection()->send(snac);
	} else if (xtraz.type() == Xtraz::Response) {
		XtrazResponse response = xtraz.response();
		if (response.serviceId() != "cAwaySrv" && response.event() != "OnRemoteNotification" &&
			response.value("uin") != contact->id())
		{
			debug() << "Skipped xtraz response" << response.serviceId() << "from" << response.value("uin");
			return;
		}
		int index = response.value("index", "-1").toInt();
		if (index > 0 && index < xstatusList()->size()) {
			setXstatus(contact,
					   response.value("title"),
					   xstatusList()->at(index).icon,
					   response.value("desc"));
			debug() << "xstatus" << contact->name() << index << xstatusList()->at(index).status;
		}
	}
}

void XStatusHandler::statusChanged(IcqContact *contact, Status &status, const TLVMap &tlvs)
{
	if (status.type() == Status::Offline)
		return;
	IcqAccount *account = contact->account();
	SessionDataItemMap statusNoteData(tlvs.value(0x1D));
	qint8 moodIndex = -1;
	if (statusNoteData.contains(0x0e)) {
		QString moodStr = statusNoteData.value(0x0e).read<QString>(Util::asciiCodec());
		if (moodStr.startsWith("icqmood")) {
			bool ok;
			moodIndex = moodStr.mid(7, -1).toInt(&ok);
			if (!ok)
				moodIndex = -1;
		}
	}
	if (handelXStatusCapabilities(contact, moodIndex)) {
		XtrazRequest request("cAwaySrv", "srvMng");
		request.setValue("id", "AwayStat");
		request.setValue("trans", "1");
		request.setValue("senderId", account->id());
		SNAC snac = request.snac(contact);
		account->connection()->send(snac);
	}
	foreach (const Capability &cap, contact->capabilities()) {
		if (qipstatuses.contains(cap)) {
			OscarStatus qipStatus = qipstatuses.value(cap);
			status.setIcon(qipStatus.icon());
			status.setName(qipStatus.name());
		}
	}
}

bool XStatusHandler::handelXStatusCapabilities(IcqContact *contact, qint8 mood)
{
	const Capabilities &caps = contact->capabilities();
	foreach(const XStatus &status, *xstatusList())
	{
		if (caps.match(status.capability) || (mood != -1 && status.mood == mood)) {
			setXstatus(contact, status.status.toString(), status.icon);
			return true;
		}
	}
	return false;
}

void XStatusHandler::removeXStatuses(Capabilities &caps)
{
	foreach (const XStatus &xstatus, *xstatusList())
		caps.removeAll(xstatus.capability);
}

void XStatusHandler::setXstatus(IcqContact *contact, const QString &title, const ExtensionIcon &icon , const QString &desc)
{
	Status status = contact->status();
	QVariantHash extStatuses = status.property("extendedStatuses", QVariantHash());
	QVariantHash extStatus;
	extStatus.insert("id", QT_TRANSLATE_NOOP("XStatus", "X-Status").toString());
	extStatus.insert("title", unescape(title));
	extStatus.insert("icon", QVariant::fromValue(icon));
	if (!desc.isNull())
		extStatus.insert("desc", unescape(desc));
	extStatuses.insert("xstatus", extStatus);
	status.setProperty("extendedStatuses", extStatuses);
	contact->setStatus(status);
}

} } // namespace qutim_sdk_0_3::oscar

QUTIM_EXPORT_PLUGIN(qutim_sdk_0_3::oscar::XStatusHandler);

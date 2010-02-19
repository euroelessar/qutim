/****************************************************************************
 *  xstatus.h
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

#ifndef XTRAZ_P_H
#define XTRAZ_P_H

#include "xtraz.h"
#include "messages.h"
#include "roster.h"
#include "oscarstatus.h"
#include <QXmlStreamReader>
#include <qutim/plugin.h>

namespace Icq
{

class QipExtendedStatus : public OscarStatus
{
public:
	QipExtendedStatus(quint16 status, const QString &iconName,
					  const LocalizedString &name, quint16 id);
};

struct XStatus
{
	XStatus() { }
	XStatus(const LocalizedString &status, const QString &icon,
			qint8 mood, const Capability &capability);
	LocalizedString status;
	ExtensionIcon icon;
	qint8 mood;
	Capability capability;
};

typedef QList<XStatus> XStatusList;

class XStatusHandler: public Plugin, public Tlv2711Plugin, public RosterPlugin
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::Tlv2711Plugin qutim_sdk_0_3::RosterPlugin)
	Q_CLASSINFO("DebugName", "ICQ/Xstatus")
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
	XStatusHandler();
	void processTlvs2711(IcqContact *contact, Capability guid, quint16 type, const DataUnit &data, const Cookie &cookie);
	void statusChanged(IcqContact *contact, Status &status, const TLVMap &tlvs);
	static bool handelXStatusCapabilities(IcqContact *contact, const Capabilities &caps, qint8 mood);
	static void removeXStatuses(Capabilities &caps);
	static QHash<Capability, OscarStatus> qipstatuses;
private:
	static void handleNotify(IcqContact *contact, const QString &message, const Cookie &cookie);
	static void parseQuery(const QString &query, QString *pluginID);
	static void parseRes(IcqContact *contact, const QString &res);
	static void parseSrv(IcqContact *contact, QXmlStreamReader &xml, bool response, const Cookie &cookie = Cookie());
	static void parseVal(IcqContact *contact, QXmlStreamReader &xml);
	static void parseAwayMsg(IcqContact *contact, QXmlStreamReader &xml);
	static void parseRequest(IcqContact *contact, QXmlStreamReader &xml, const Cookie &cookie);
	static void sendXStatus(IcqContact *contact, const Cookie &cookie);

};

} // namespace Icq

#endif // XTRAZ_P_H

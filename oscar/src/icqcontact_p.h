/****************************************************************************
 *  icqcontact_p.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "icqcontact.h"

#ifndef ICQCONTACT_PH_H
#define ICQCONTACT_PH_H

#include "capability.h"
#include "oscarconnection.h"
#include "feedbag.h"
#include "qutim/inforequest.h"
#include <QDateTime>

namespace qutim_sdk_0_3 {

namespace oscar {

typedef QPair<LocalizedString, QVariant> InfoField;
typedef QHash<QString, InfoField> InfoFieldList;

enum SsiBuddyTlvs
{
	SsiBuddyNick = 0x0131,
	SsiBuddyComment = 0x013c,
	SsiBuddyReqAuth = 0x0066,
	SsiBuddyTags = 0x023c
};

enum ContactCapabilityFlags
{
	html_support      = 0x0001,
	utf8_support      = 0x0002,
	srvrelay_support  = 0x0004
};

class IcqInfoRequest : public InfoRequest
{
	Q_OBJECT
public:
	IcqInfoRequest(IcqContact *contact);
	virtual ~IcqInfoRequest();
	virtual InfoItem item(const QString &name = QString()) const;
	virtual State state() const;
private slots:
	void onDone(bool ok);
private:
	QPointer<FullInfoMetaRequest> m_metaReq;
	MetaInfoValuesHash m_values;
	State m_state;
	IcqContact *m_contact;
};

struct IcqContactPrivate
{
	Q_DECLARE_PUBLIC(IcqContact);
	void clearCapabilities();
	void requestNick();
	void setCapabilities(const Capabilities &caps);
	void setChatState(ChatState state);
	FeedbagItem getNotInListGroup();
	Q_DECLARE_FLAGS(CapabilityFlags, ContactCapabilityFlags)
	IcqAccount *account;
	QString uin;
	QString name;
	Status status;
	QString avatar;
	quint16 version;
	CapabilityFlags flags;
	Capabilities capabilities;
	DirectConnectionInfo dc_info;
	QList<FeedbagItem> items;
	QStringList tags;
	ChatState state;
	InfoFieldList fields;
	QDateTime onlineSince;
	QDateTime awaySince;
	QDateTime regTime;
	IcqContact *q_ptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(IcqContactPrivate::CapabilityFlags)

} } // namespace qutim_sdk_0_3::oscar

#endif // ICQCONTACT_PH_H

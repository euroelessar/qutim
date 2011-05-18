/****************************************************************************
 *  xtraz.h
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

#ifndef XTRAZ_H
#define XTRAZ_H

#include "icq_global.h"
#include "messages.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqContact;
class XtrazPrivate;
class XtrazRequestPrivate;
class XtrazResponsePrivate;

enum XtrazType
{
	xtrazInvitation = 0x0001,
	xtrazData       = 0x0002,
	xtrazUserRemove = 0x0004,
	xtrazNotify     = 0x0008
};

const Capability MSG_XSTRAZ_SCRIPT(0x3b60b3ef, 0xd82a6c45, 0xa4e09c5a, 0x5e67e865);

class LIBOSCAR_EXPORT XtrazRequest
{
public:
	XtrazRequest(const QString &serviceId = QString(), const QString &pluginId = QString());
	XtrazRequest(const XtrazRequest &data);
	~XtrazRequest();
	XtrazRequest &operator=(const XtrazRequest &rhs);
	QString pluginId() const;
	void setPluginId(const QString &pluginId);
	QString serviceId() const;
	void setServiceId(const QString &serviceId);
	QString value(const QString &name, const QString &def = QString()) const;
	bool contains(const QString &name) const;
	void setValue(const QString &name, const QString &value);
	SNAC snac(IcqContact *contact) const;
private:
	friend class Xtraz;
	QSharedDataPointer<XtrazRequestPrivate> d;
};

class LIBOSCAR_EXPORT XtrazResponse
{
public:
	XtrazResponse(const QString &serviceId = QString(), const QString &event = QString());
	XtrazResponse(const XtrazResponse &data);
	~XtrazResponse();
	XtrazResponse &operator=(const XtrazResponse &rhs);
	QString event() const;
	void setEvent(const QString &event);
	QString serviceId() const;
	void setServiceId(const QString &serviceId);
	QString value(const QString &name, const QString &def = QString()) const;
	bool contains(const QString &name) const;
	void setValue(const QString &name, const QString &value);
	SNAC snac(IcqContact *contact, quint64 cookie) const;
private:
	friend class Xtraz;
	QSharedDataPointer<XtrazResponsePrivate> d;
};

class LIBOSCAR_EXPORT Xtraz
{
public:
	enum Type
	{
		Response,
		Request,
		Invalid
	};
	Xtraz(const QString &message);
	~Xtraz();
	Xtraz &operator=(const Xtraz &rhs);
	Type type();
	XtrazRequest request();
	XtrazResponse response();
private:
	Xtraz();
	Xtraz(const Xtraz &xtraz);
	QSharedDataPointer<XtrazPrivate> d;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // XTRAZ_H

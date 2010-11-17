/****************************************************************************
 *  abstractmetarequest.cpp
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

#include "abstractmetarequest_p.h"
#include "snac.h"
#include "icqaccount.h"
#include "connection.h"
#include "metainfo_p.h"

namespace qutim_sdk_0_3 {

namespace oscar {

AbstractMetaRequest::AbstractMetaRequest()
{
}

AbstractMetaRequest::~AbstractMetaRequest()
{
	cancel();
}

quint16 AbstractMetaRequest::id() const
{
	return d_func()->id;
}

IcqAccount *AbstractMetaRequest::account() const
{
	return d_func()->account;
}

bool AbstractMetaRequest::isDone() const
{
	return d_func()->ok;
}

void AbstractMetaRequest::setTimeout(int msec)
{
	d_func()->timer.setInterval(msec);
}

void AbstractMetaRequest::cancel()
{
	close(false);
}

AbstractMetaRequest::AbstractMetaRequest(IcqAccount *account, AbstractMetaRequestPrivate *d) :
	d_ptr(d)
{
	d->id = MetaInfo::instance().nextId();
	d->account = account;
	d->ok = false;
	d->timer.setInterval(60000);
	d->timer.setSingleShot(true);
	connect(&d->timer, SIGNAL(timeout()), this, SLOT(cancel()));
}

void AbstractMetaRequest::sendRequest(quint16 type, const DataUnit &extendData) const
{
	Q_D(const AbstractMetaRequest);
	SNAC snac(ExtensionsFamily, ExtensionsMetaCliRequest);
	DataUnit tlvData;
	DataUnit data;
	data.append<quint32>(d->account->id().toUInt(), LittleEndian);
	data.append<quint16>(0x07d0, LittleEndian);
	data.append<quint16>(d->id, LittleEndian);
	data.append<quint16>(type, LittleEndian);
	data.append(extendData.data());
	tlvData.append<quint16>(data.data().size(), LittleEndian);
	tlvData.append(data.data());
	snac.appendTLV(1, tlvData);
	MetaInfo::instance().addRequest(const_cast<AbstractMetaRequest*>(this));
	d->account->connection()->send(snac);
	d->timer.start();
}

void AbstractMetaRequest::close(bool ok)
{
	Q_D(AbstractMetaRequest);
	d->ok = ok;
	if (MetaInfo::instance().removeRequest(this) || ok)
		emit done(ok);
}

QString AbstractMetaRequest::readSString(const DataUnit &data)
{
	QString str = data.read<QString, quint16>(Util::asciiCodec(), LittleEndian);
	if (str.endsWith(QChar('\0')))
		str.chop(1);
	return str;
}

} } // namespace qutim_sdk_0_3::oscar

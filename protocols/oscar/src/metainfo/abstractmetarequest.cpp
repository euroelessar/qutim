/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "abstractmetarequest_p.h"
#include "../snac.h"
#include "../icqaccount.h"
#include "../connection.h"
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

AbstractMetaRequest::ErrorType AbstractMetaRequest::errorType()
{
	return d_func()->errorType;
}

QString AbstractMetaRequest::errorString()
{
	return d_func()->errorString;
}

void AbstractMetaRequest::cancel()
{
	close(false, Canceled, tr("The metarequest cancelled by user"));
}

void AbstractMetaRequest::timeout()
{
	close(false, Timeout, tr("The server did not answer on the metarequest"));
}

AbstractMetaRequest::AbstractMetaRequest(IcqAccount *account, AbstractMetaRequestPrivate *d) :
	d_ptr(d)
{
	d->id = MetaInfo::instance().nextId();
	d->account = account;
	d->ok = false;
	d->timer.setInterval(60000);
	d->timer.setSingleShot(true);
	d->errorType = NoError;
	connect(&d->timer, SIGNAL(timeout()), this, SLOT(timeout()));
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

void AbstractMetaRequest::close(bool ok, ErrorType error, const QString &errorString)
{
	Q_D(AbstractMetaRequest);
	d->ok = ok;
	d->errorType = error;
	d->errorString = errorString;
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


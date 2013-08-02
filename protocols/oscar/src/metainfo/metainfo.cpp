/****************************************************************************
**
** qutIM - instant messenger
**
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

#include "metainfo_p.h"
#include "../icqaccount.h"
#include "../icqcontact.h"
#include "../icqprotocol.h"
#include "../oscarconnection.h"
#include "metafields_p.h"
#include <QStringList>
#include <QDate>
#include <QEventLoop>

namespace qutim_sdk_0_3 {

namespace oscar {

MetaInfo *MetaInfo::self = 0;

MetaInfo::MetaInfo() :
	m_sequence(0)
{
	Q_ASSERT(!self);
	self = this;
	m_infos << SNACInfo(ExtensionsFamily, ExtensionsMetaSrvReply)
		<< SNACInfo(ExtensionsFamily, ExtensionsMetaError);
	connect(IcqProtocol::instance(), SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
			this, SLOT(onNewAccount(qutim_sdk_0_3::Account*)));
	foreach (Account *account, IcqProtocol::instance()->accounts())
		onNewAccount(account);
}

void MetaInfo::handleSNAC(AbstractConnection *conn, const SNAC &snac)
{
	Q_UNUSED(conn);
	if (snac.family() == ExtensionsFamily && snac.subtype() == ExtensionsMetaSrvReply) {
		TLVMap tlvs = snac.read<TLVMap>();
		if (tlvs.contains(0x01)) {
			DataUnit data(tlvs.value(0x01));
			data.skipData(6); // skip field length + my uin
			quint16 metaType = data.read<quint16>(LittleEndian);
			if (metaType == 0x07da) {
				quint16 reqNumber = data.read<quint16>(LittleEndian);
				QHash<quint16, AbstractMetaRequest*>::iterator itr = m_requests.find(reqNumber);
				quint16 dataType = data.read<quint16>(LittleEndian);
				quint8 success = data.read<quint8>(LittleEndian);
				if (itr == m_requests.end()) {
					qDebug() << "Unexpected metainfo response" << reqNumber;
					return;
				}
				if (success == 0x0a) {
					if (!itr.value()->handleData(dataType, data))
						qDebug() << "Unexpected metainfo response with type" << hex << dataType;
				} else {
					qDebug() << "Meta request failed" << hex << success;
					itr.value()->close(false, AbstractMetaRequest::ProtocolError, tr("Incorrect format of the metarequest"));
				}
			}
		}
	} else if (snac.family() == ExtensionsFamily && snac.subtype() == ExtensionsMetaError) {
		ProtocolError error(snac);
		qDebug() << QString("MetaInfo service error (%1, %2): %3")
				.arg(error.code(), 2, 16)
				.arg(error.subcode(), 2, 16)
				.arg(error.errorString());
		if (error.tlvs().contains(0x21)) {
			DataUnit data(error.tlvs().value(0x21));
			data.skipData(6); // skip field length + my uin
			quint16 metaType = data.read<quint16>(LittleEndian);
			if (metaType == 0x07d0) {
				quint16 reqNumber = data.read<quint16>(LittleEndian);
				AbstractMetaRequest *request = m_requests.value(reqNumber);
				if (request) {
					request->close(false, AbstractMetaRequest::ProtocolError, error.errorString());
				}
			}
		}
	}
}

void MetaInfo::addRequest(AbstractMetaRequest *request)
{
	m_requests.insert(request->id(), request);
}

bool MetaInfo::removeRequest(AbstractMetaRequest *request)
{
	return m_requests.remove(request->id()) > 0;
}

void MetaInfo::onNewAccount(qutim_sdk_0_3::Account *account)
{
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
}

void MetaInfo::onAccountStatusChanged(const qutim_sdk_0_3::Status &status)
{
	if (status == Status::Offline) {
		QHash<quint16, AbstractMetaRequest*> requests = m_requests;
		foreach (AbstractMetaRequest *req, requests)
			req->close(false);
		Q_ASSERT(m_requests.isEmpty());
	}
}

} } // namespace qutim_sdk_0_3::oscar


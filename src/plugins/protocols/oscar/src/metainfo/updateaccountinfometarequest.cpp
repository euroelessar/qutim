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

#include "updateaccountinfometarequest.h"
#include "tlvbasedmetarequest_p.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class UpdateAccountInfoMetaRequestPrivate : public TlvBasedMetaRequestPrivate
{

};

UpdateAccountInfoMetaRequest::UpdateAccountInfoMetaRequest(IcqAccount *account, const MetaInfoValuesHash &values) :
	TlvBasedMetaRequest(account, new TlvBasedMetaRequestPrivate)
{
	Q_D(UpdateAccountInfoMetaRequest);
	d->values = values;
}

void UpdateAccountInfoMetaRequest::send() const
{
	sendTlvBasedRequest(0x0C3A);
}

bool UpdateAccountInfoMetaRequest::handleData(quint16 type, const DataUnit &data)
{
	Q_UNUSED(data);
	if (type == 0x0c3f) {
		qDebug() << "Account info successfully has been updated";
		emit infoUpdated();
		return true;
	}
	return false;
}

} } // namespace qutim_sdk_0_3::oscar


/****************************************************************************
 *  updateaccountinfometarequest.cpp
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
		debug() << "Account info successfully has been updated";
		emit infoUpdated();
		return true;
	}
	return false;
}

} } // namespace qutim_sdk_0_3::oscar

/****************************************************************************
 *  updateaccountinfometarequest.h
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

#ifndef UPDATEACCOUNTINFOMETAREQUEST_H
#define UPDATEACCOUNTINFOMETAREQUEST_H

#include "tlvbasedmetarequest.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class UpdateAccountInfoMetaRequestPrivate;

class LIBOSCAR_EXPORT UpdateAccountInfoMetaRequest : public TlvBasedMetaRequest
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(UpdateAccountInfoMetaRequest)
public:
	UpdateAccountInfoMetaRequest(IcqAccount *account, const MetaInfoValuesHash &values = MetaInfoValuesHash());
	virtual void send() const;
signals:
	void infoUpdated();
protected:
	virtual bool handleData(quint16 type, const DataUnit &data);
};

} } // namespace qutim_sdk_0_3::oscar

#endif // UPDATEACCOUNTINFOMETAREQUEST_H

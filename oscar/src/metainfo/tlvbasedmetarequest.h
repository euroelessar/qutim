/****************************************************************************
 *  tlvbasedmetarequest.h
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

#ifndef TLVBASEDMETAREQUEST_H
#define TLVBASEDMETAREQUEST_H

#include "abstractmetarequest.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqAccount;
class IcqContact;
class TlvBasedMetaRequestPrivate;

class LIBOSCAR_EXPORT TlvBasedMetaRequest : public AbstractMetaRequest
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(TlvBasedMetaRequest)
public:
	void setValue(const MetaField &field, const QVariant &value);
protected:
	TlvBasedMetaRequest(IcqAccount *account, TlvBasedMetaRequestPrivate *d);
	void sendTlvBasedRequest(quint16 type) const;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // TLVBASEDMETAINFOREQUEST_H

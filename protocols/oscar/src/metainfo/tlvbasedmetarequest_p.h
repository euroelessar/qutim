/****************************************************************************
 *  tlvbasedmetarequest_p.h
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

#ifndef TLVBASEDMETAREQUEST_P_H
#define TLVBASEDMETAREQUEST_P_H

#include "tlvbasedmetarequest.h"
#include "abstractmetarequest_p.h"
#include "tlv.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class TlvBasedMetaRequestPrivate : public AbstractMetaRequestPrivate
{
public:
	void addString(quint16 id, MetaFieldEnum value, DataUnit &data, bool test = true) const;
	void addString(const QString &str, DataUnit &data) const;
	template <typename T>
	void addField(quint16 id, MetaFieldEnum value, DataUnit &data, bool test = true) const;
	template <typename T>
	void addCategoryId(quint16 id, MetaFieldEnum value, DataUnit &data, const FieldNamesList &list) const;
	void addCategory(quint16 id, MetaFieldEnum value, DataUnit &data, const FieldNamesList &list) const;
	MetaInfoValuesHash values;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // TLVBASEDMETAINFOREQUEST_P_H

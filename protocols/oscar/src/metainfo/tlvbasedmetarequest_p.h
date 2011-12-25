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

#ifndef TLVBASEDMETAREQUEST_P_H
#define TLVBASEDMETAREQUEST_P_H

#include "tlvbasedmetarequest.h"
#include "abstractmetarequest_p.h"
#include "../tlv.h"

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


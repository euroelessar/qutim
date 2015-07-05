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

#ifndef INFOMETAREQUEST_P_H
#define INFOMETAREQUEST_P_H

#include "infometarequest.h"
#include "abstractmetarequest_p.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class ShortInfoMetaRequestPrivate : public AbstractMetaRequestPrivate
{
public:
	MetaInfoValuesHash values;
	quint32 uin;
	inline void readString(MetaFieldEnum value, const DataUnit &data);
	inline void readFlag(MetaFieldEnum value, const DataUnit &data);
	void dump();
};

class FullInfoMetaRequestPrivate : public ShortInfoMetaRequestPrivate
{
public:
	template <typename T>
	void readField(MetaFieldEnum value, const DataUnit &data, const FieldNamesList &list);
	void readCatagories(MetaFieldEnum value, const DataUnit &data, const FieldNamesList &list);
	void handleBasicInfo(const DataUnit &data);
	void handleMoreInfo(const DataUnit &data);
	void handleEmails(const DataUnit &data);
	void handleHomepage(const DataUnit &data);
	void handleWork(const DataUnit &data);
};

} } // namespace qutim_sdk_0_3::oscar

#endif // INFOMETAREQUEST_P_H


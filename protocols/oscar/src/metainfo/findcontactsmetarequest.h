/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef FINDCONTACTSMETAREQUEST_H
#define FINDCONTACTSMETAREQUEST_H

#include "tlvbasedmetarequest.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class FindContactsMetaRequestPrivate;

class LIBOSCAR_EXPORT FindContactsMetaRequest : public TlvBasedMetaRequest
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(FindContactsMetaRequest)
public:
	enum Status {
		Offline = 0,
		Online = 1,
		NonWebaware
	};
	struct FoundContact
	{
		FoundContact();
		QString uin;
		QString nick;
		QString firstName;
		QString lastName;
		QString email;
		bool authFlag;
		Status status;
		QString gender;
		quint16 age;
	};

	FindContactsMetaRequest(IcqAccount *account, const MetaInfoValuesHash &values = MetaInfoValuesHash());
	virtual void send() const;
	const QHash<QString, FoundContact> &contacts() const;
signals:
	void contactFound(const FindContactsMetaRequest::FoundContact &contact);
protected:
	virtual bool handleData(quint16 type, const DataUnit &data);
};

} } // namespace qutim_sdk_0_3::oscar


#endif // FINDCONTACTSMETAREQUEST_H


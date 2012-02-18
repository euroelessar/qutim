/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef OSCARCONTACTSEARCH_H
#define OSCARCONTACTSEARCH_H

#include "qutim/contactsearch.h"
#include "metainfo/findcontactsmetarequest.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class OscarContactSearch : public ContactSearchRequest
{
	Q_OBJECT
public:
	OscarContactSearch(IcqAccount *account);
	virtual DataItem fields() const;
	virtual void start(const DataItem &fields);
	virtual void cancel();
	virtual int columnCount() const;
	virtual QVariant headerData(int column, int role = Qt::DisplayRole);
	virtual int rowCount() const;
	virtual QVariant data(int row, int column, int role = Qt::DisplayRole);
	virtual Contact *contact(int row);
private slots:
	void onNewContact(const FindContactsMetaRequest::FoundContact &contact);
	void onDone(bool ok);
private:
	void addField(DataItem &item, const MetaField &field) const;
	QScopedPointer<FindContactsMetaRequest> m_request;
	QList<FindContactsMetaRequest::FoundContact> m_contacts;
	QString m_uin;
	IcqAccount *m_account;
};

class OscarContactSearchFactory : public GeneralContactSearchFactory
{
	Q_OBJECT
	Q_CLASSINFO("DependsOn", "qutim_sdk_0_3::oscar::IcqProtocol")
public:
	OscarContactSearchFactory();
	virtual ContactSearchRequest *request(const QString &name) const;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // OSCARCONTACTSEARCH_H


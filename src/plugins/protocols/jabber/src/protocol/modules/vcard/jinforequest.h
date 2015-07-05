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
#ifndef JINFOREQUEST_H
#define JINFOREQUEST_H

#include <qutim/inforequest.h>
#include <jreen/vcardmanager.h>
#include <jreen/iq.h>

namespace Jabber
{
using namespace qutim_sdk_0_3;

class JVCardManager;
class JInfoRequestPrivate;

class JInfoRequest : public InfoRequest
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JInfoRequest)
public:
	enum DataType
	{
		Nick,
		FirstName,
		MiddleName,
		LastName,
		Birthday,
		Homepage,
		HomePhone,
		WorkPhone,
		MobilePhone,
		Phone,
		PersonalEmail,
		WorkEmail,
		Email,
		HomeAddress,
		WorkAddress,
		Address,
		Country,
		Region,
		City,
		Postcode,
		Street,
		ExtendedAddress,
		Postbox,
		OrgName,
		OrgUnit,
		Title,
		Role,
		About,
		Features
	};

	JInfoRequest(Jreen::VCardManager *manager, QObject *object);
	~JInfoRequest();

protected slots:
	void setFetchedVCard(const Jreen::VCard::Ptr &vcard);

protected:
	virtual void doRequest(const QSet<QString> &hints);
	virtual void doUpdate(const DataItem &dataItem);
	virtual void doCancel();
	virtual DataItem createDataItem() const;
private:
	Jreen::VCard::Ptr convert(const DataItem &item) const;
	static DataItem telephoneItem(const Jreen::VCard::Telephone &phone);
	static DataItem emailItem(const Jreen::VCard::EMail &email);
	DataItem addressItem(const Jreen::VCard::Address &addr) const;
	static DataType getPhoneType(const Jreen::VCard::Telephone &phone);
	static DataType getEmailType(const Jreen::VCard::EMail &email);
	static DataType getAddressType(const Jreen::VCard::Address &address);
	void addItem(DataType type, DataItem &group, const QVariant &data) const;
	void addMultilineItem(DataType type, DataItem &group, const QString &data) const;
	void addItemList(DataType type, DataItem &group, const QString &data) const;
	void addItemList(DataType type, DataItem &group, const QStringList &data) const;
private slots:
	void onStoreFinished();
private:
	QScopedPointer<JInfoRequestPrivate> d_ptr;
};
}

#endif // JINFOREQUEST_H


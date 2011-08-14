#ifndef JINFOREQUEST_H
#define JINFOREQUEST_H

#include <qutim/inforequest.h>
#include <jreen/vcardmanager.h>

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
	
	JInfoRequest(Jreen::VCardReply *reply);
	~JInfoRequest();
	static Jreen::VCard::Ptr convert(const DataItem &item);
	DataItem item(const QString &name) const;
	State state() const;

protected slots:
	void setFetchedVCard(const Jreen::VCard::Ptr &vcard);

private:
	void addItem(DataType type, DataItem &group, const QVariant &data);
	void addMultilineItem(DataType type, DataItem &group, const QString &data);
	void addItemList(DataType type, DataItem &group, const QString &data);
	void addItemList(DataType type, DataItem &group, const QStringList &data);
	QScopedPointer<JInfoRequestPrivate> d_ptr;
};
}

#endif // JINFOREQUEST_H

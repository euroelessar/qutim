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

	JInfoRequest(QObject *object, Jreen::VCardReply *reply);
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
	void onIqReceived(const Jreen::IQ &iq, int);
private:
	QScopedPointer<JInfoRequestPrivate> d_ptr;
};
}

#endif // JINFOREQUEST_H

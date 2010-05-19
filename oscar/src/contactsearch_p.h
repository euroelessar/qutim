#ifndef OSCARCONTACTSEARCH_H
#define OSCARCONTACTSEARCH_H

#include "qutim/contactsearch.h"
#include "metainfo.h"

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
	virtual int contactCount() const;
	virtual QVariant data(int row, int column, int role = Qt::DisplayRole);
	virtual Contact *contact(int row);
private slots:
	void onNewContact(const FindContactsMetaRequest::FoundContact &contact);
	void onDone(bool ok);
private:
	void addField(DataItem &item, const MetaInfoField &field) const;
	void testResult();
	QScopedPointer<FindContactsMetaRequest> m_request;
	QList<FindContactsMetaRequest::FoundContact> m_contacts;
	QString m_uin;
	IcqAccount *m_account;
};

class OscarContactSearchFactory : public GeneralContactSearchFactory
{
	Q_OBJECT
public:
	OscarContactSearchFactory();
	virtual ContactSearchRequest *request(const QString &name) const;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // OSCARCONTACTSEARCH_H

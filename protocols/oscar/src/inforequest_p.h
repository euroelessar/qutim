#ifndef OSCAR_INFOREQUEST_H
#define OSCAR_INFOREQUEST_H

#include "qutim/inforequest.h"
#include "metainfo/infometarequest.h"
#include <QPointer>

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqContact;

class IcqInfoRequest : public InfoRequest
{
	Q_OBJECT
public:
	IcqInfoRequest(IcqAccount *account);
	IcqInfoRequest(IcqContact *contact);
	virtual ~IcqInfoRequest();
	virtual DataItem item(const QString &name = QString()) const;
	virtual State state() const;
private slots:
	void onDone(bool ok);
private:
	void addItem(const MetaField &field, DataItem &group) const;
	void init();
	QPointer<FullInfoMetaRequest> m_metaReq;
	MetaInfoValuesHash m_values;
	State m_state;
	bool m_accountInfo;
	union {
		IcqAccount *m_account;
		IcqContact *m_contact;
	};
};

} } // namespace qutim_sdk_0_3::oscar

#endif // OSCAR_INFOREQUEST_H

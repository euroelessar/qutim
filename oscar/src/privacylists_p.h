#ifndef PRIVACYLISTS_H
#define PRIVACYLISTS_H

#include "feedbag.h"
#include <qutim/actiongenerator.h>

namespace qutim_sdk_0_3 {

namespace oscar {


class PrivateListActionGenerator : public ActionGenerator
{
public:
	PrivateListActionGenerator(quint16 type, const QIcon &icon,
				const LocalizedString &text1, const LocalizedString &text2);
	virtual ~PrivateListActionGenerator();
protected:
	virtual QObject *generateHelper() const;
private:
	quint16 m_type;
	LocalizedString m_text2;
};

class PrivacyLists : public QObject, public FeedbagItemHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::FeedbagItemHandler)
public:
    PrivacyLists();
	static PrivacyLists *instance() { Q_ASSERT(self); return self; }
	bool handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error);
private slots:
	void onModifyPrivateList();
private:
	static PrivacyLists *self;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // PRIVACYLISTS_H

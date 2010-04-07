#ifndef AUTHORIZATION_P_H
#define AUTHORIZATION__PH

#include "snachandler.h"
#include "feedbag.h"
#include <qutim/actiongenerator.h>

namespace qutim_sdk_0_3 {

namespace oscar {

class AuthorizeActionGenerator : public ActionGenerator
{
public:
	AuthorizeActionGenerator();
protected:
	virtual QObject *generateHelper() const;
};

class Authorization : public QObject, public SNACHandler, public FeedbagItemHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::SNACHandler qutim_sdk_0_3::oscar::FeedbagItemHandler)
public:
    Authorization();
	static Authorization *instance() { Q_ASSERT(self); return self; }
protected:
	void handleSNAC(AbstractConnection *conn, const SNAC &snac);
	bool handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error);
private slots:
	void sendAuthResponse(bool auth);
	void onSendRequestClicked();
	void sendAuthRequest();
private:
	static Authorization *self;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // AUTHORIZATION_H

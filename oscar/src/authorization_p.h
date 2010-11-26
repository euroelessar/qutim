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
	virtual void showImpl(QAction *action, QObject *object);
};

class Authorization : public QObject, public SNACHandler, public FeedbagItemHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::SNACHandler qutim_sdk_0_3::oscar::FeedbagItemHandler)
	Q_CLASSINFO("DependsOn", "qutim_sdk_0_3::oscar::IcqProtocol")
public:
    Authorization();
	static Authorization *instance() { Q_ASSERT(self); return self; }
protected:
	void handleSNAC(AbstractConnection *conn, const SNAC &snac);
	bool handleFeedbagItem(Feedbag *feedbag, const FeedbagItem &item, Feedbag::ModifyType type, FeedbagError error);
private slots:
	void onSendRequestClicked(QObject *object);
private:
	static Authorization *self;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // AUTHORIZATION_H


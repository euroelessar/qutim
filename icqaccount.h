#ifndef ICQACCOUNT_H
#define ICQACCOUNT_H

#include <qutim/account.h>

using namespace qutim_sdk_0_3;

struct IcqAccountPrivate;
class Roster;

class IcqAccount : public Account
{
	Q_OBJECT
public:
	IcqAccount(const QString &uin);
	virtual ~IcqAccount();
	virtual void setStatus(Status status);
	Roster *roster();
private:
	QScopedPointer<IcqAccountPrivate> p;
};

#endif // ICQACCOUNT_H

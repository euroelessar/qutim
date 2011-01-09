#ifndef ASTRALACCOUNT_H
#define ASTRALACCOUNT_H

#include <qutim/account.h>
#include <TelepathyQt4/ConnectionManager>
#include <TelepathyQt4/Connection>
#include <TelepathyQt4/TextChannel>
#include <TelepathyQt4/AccountManager>
#include <TelepathyQt4/PendingConnection>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingReady>

#include "astralroster.h"

using namespace qutim_sdk_0_3;
using namespace Tp;

class AstralProtocol;
class AstralSessionManager;
struct AstralAccountPrivate;

class AstralAccount : public qutim_sdk_0_3::Account
{
	Q_OBJECT
public:
	AstralAccount(AstralProtocol *protocol, const QString &id);
	virtual ~AstralAccount();
	virtual void setStatus(Status status);
	virtual ChatUnit *getUnitForSession(ChatUnit *unit);
	virtual ChatUnit *getUnit(const QString &unitId, bool create);
	AstralRoster *roster();
	AstralSessionManager *sessionManager();
private slots:
	void onConnect(Tp::PendingOperation *op);
	void onConnectError(Tp::PendingOperation *op);
	void onNewChannels(const Tp::ChannelDetailsList &channels);
private:
	friend class AstralRoster;
	QScopedPointer<AstralAccountPrivate> p;
};

#endif // ASTRALACCOUNT_H

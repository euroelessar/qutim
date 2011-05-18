#ifndef ASTRALSESSION_H
#define ASTRALSESSION_H

#include <qutim/chatunit.h>
#include <TelepathyQt4/TextChannel>
#include <TelepathyQt4/Message>

using namespace qutim_sdk_0_3;
using namespace Tp;

struct AstralSessionPrivate;
class AstralAccount;

class AstralSession : public ChatUnit
{
	Q_OBJECT
public:
	AstralSession(const TextChannelPtr &channel, AstralAccount *acc);
	virtual ~AstralSession();
	virtual QString id() const;
	virtual bool sendMessage(const qutim_sdk_0_3::Message &message);
private slots:
	void onMessageReceived(const Tp::ReceivedMessage &message);
	void onMessageSent(Tp::PendingOperation *operation);
private:
	QScopedPointer<AstralSessionPrivate> p;
};

#endif // ASTRALSESSION_H

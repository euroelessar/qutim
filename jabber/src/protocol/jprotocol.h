#ifndef JPROTOCOL_H
#define JPROTOCOL_H

#include <qutim/protocol.h>
#include <qutim/status.h>
#include <jreen/presence.h>
#include <jreen/abstractroster.h>

namespace Jabber
{

using namespace qutim_sdk_0_3;

class JProtocolPrivate;
class JAccount;

class JProtocol : public Protocol
{
	Q_OBJECT
	Q_CLASSINFO("Protocol", "jabber")
	Q_CLASSINFO("Uses", "AuthorizationService")
	Q_DECLARE_PRIVATE(JProtocol)
public:
	JProtocol();
	virtual ~JProtocol();
	static inline JProtocol *instance() {
		if(!self) qWarning("JProtocol isn't created");
		return self;
	}
	virtual QList<Account *> accounts() const;
	virtual Account *account(const QString &id) const;
	void addAccount(JAccount *account, bool isEmit = false);
	virtual QVariant data(DataType type);
	bool event(QEvent *ev);
private slots:
	void onKickUser(QObject* obj);
	void onBanUser(QObject* obj);
	void onConvertToMuc(QObject* obj);
	void onJoinLeave(QObject *obj);
	void onShowConfigDialog(QObject *obj);
	void onSaveRemoveBookmarks(QObject *obj);
	void removeAccount(QObject *obj);
	void onChangeSubscription(QObject *obj);
private:
	void loadActions();
	virtual void loadAccounts();
	static JProtocol *self;
	QScopedPointer<JProtocolPrivate> d_ptr;

	Q_PRIVATE_SLOT(d_func(), void _q_status_changed(qutim_sdk_0_3::Status))
	Q_PRIVATE_SLOT(d_func(), void _q_subscription_changed(Jreen::AbstractRosterItem::SubscriptionType))
	Q_PRIVATE_SLOT(d_func(), void _q_conference_join_changed())
	Q_PRIVATE_SLOT(d_func(), void _q_conference_bookmark_changed())
};

namespace JStatus //TODO may be need class JStatus
{
	Jreen::Presence::Type statusToPresence(const Status &status);
	Status presenceToStatus(Jreen::Presence::Type presence);
}


} // Jabber namespace

#endif // JPROTOCOL_H

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#ifndef JPROTOCOL_H
#define JPROTOCOL_H

#include <qutim/protocol.h>
#include <qutim/status.h>
#include <qutim/debug.h>
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
		if(!self) qWarning() << "JProtocol isn't created";
		return self;
	}
	virtual QList<Account *> accounts() const;
	virtual Account *account(const QString &id) const;
	Account *doCreateAccount(const QString &id, const QVariantMap &parameters);
	void addAccount(JAccount *account, bool loadSettings);
	virtual QVariant data(DataType type);
	bool event(QEvent *ev);
protected:
	virtual void virtual_hook(int id, void *data);
private slots:
	void onKickUser(QObject* obj);
	void onKickReasonSelected(const QString &reason);
	void onBanUser(QObject* obj);
	void onBanReasonSelected(const QString &reason);
	void onConvertToMuc(QObject* obj);
	void onShowConfigDialog(QObject *obj);
	void onSaveRemoveBookmarks(QObject *obj);
	void onAccountDestroyed(QObject *obj);
	void onChangeSubscription(QObject *obj);
private:
	void loadActions();
	virtual void loadAccounts();
	static JProtocol *self;
	QScopedPointer<JProtocolPrivate> d_ptr;

	Q_PRIVATE_SLOT(d_func(), void _q_status_changed(qutim_sdk_0_3::Status))
	Q_PRIVATE_SLOT(d_func(), void _q_subscription_changed(Jreen::RosterItem::SubscriptionType))
	Q_PRIVATE_SLOT(d_func(), void _q_conference_bookmark_changed())
};

namespace JStatus //TODO may be need class JStatus
{
	Jreen::Presence::Type statusToPresence(const Status &status);
	Status presenceToStatus(Jreen::Presence::Type presence);
}


} // Jabber namespace

#endif // JPROTOCOL_H


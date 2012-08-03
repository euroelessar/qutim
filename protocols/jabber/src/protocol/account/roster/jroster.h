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
#ifndef JROSTER_H
#define JROSTER_H

#include <qutim/contact.h>
#include <qutim/configbase.h>
#include <qutim/chatsession.h>
#include "metacontacts.h"
//Jreen
#include <jreen/abstractroster.h>
#include <jreen/message.h>
#include <jreen/metacontactstorage.h>


namespace Jabber
{
using namespace qutim_sdk_0_3;
class JAccount;
class JContact;
class JContactResource;

class JRosterPrivate;
class JRoster : public Jreen::AbstractRoster
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(JRoster)
	Q_CLASSINFO("DebugName", "Jabber::Roster")
public:
	JRoster(JAccount *account);
	virtual ~JRoster();
	void loadFromStorage();
	bool ignoreChanges() const;
	ChatUnit *contact(const Jreen::JID &id, bool create = false);
	ChatUnit *selfContact(const QString &id);
	QList<JContactResource*> resources() const;
	void addContact(const JContact *contact);
	void removeContact(const JContact *contact);
	void requestSubscription(const Jreen::JID &id, const QString &reason = QString());
	void removeSubscription(const JContact *contact);
	void setName(const JContact *contact, const QString &name);
	void setGroups(const JContact *contact, const QStringList &groups);
	void handleChange(JContact *contact, const QString &metaTag);
	bool event(QEvent *ev);
public slots:
	void loadSettings();
	void saveSettings();
protected:
	virtual void onItemAdded(QSharedPointer<Jreen::RosterItem> item);
	virtual void onItemUpdated(QSharedPointer<Jreen::RosterItem> item);
	virtual void onItemRemoved(const QString &jid);
	virtual void onLoaded(const QList<QSharedPointer<Jreen::RosterItem> > &items);
	void fillContact(JContact *contact, QSharedPointer<Jreen::RosterItem> item);
	void handleSelfPresence(Jreen::Presence presence);
	void syncMetaContacts();
protected slots:
	void handleNewPresence(Jreen::Presence);
	void handleSubscription(Jreen::Presence subscribe); //TODO may be need a separated subscription manager?
	void onDisconnected();
	void onNewMessage(Jreen::Message message); //TODO move this method to JMessageManager
	void onMessageDecrypted(qutim_sdk_0_3::ChatUnit *unit, qutim_sdk_0_3::ChatUnit *unitForSession, const Jreen::Message &message);
	void onContactDestroyed(QObject *obj);
	void onMetaContactsReceived(const Jreen::MetaContactStorage::ItemList &items);
private:
	JContact *createContact(const Jreen::JID &id);
	QScopedPointer<JRosterPrivate> d_ptr;
};

}
#endif // JROSTER_H


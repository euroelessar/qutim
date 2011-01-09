#include "astralroster.h"
#include "astralaccount.h"
#include <TelepathyQt4/Types>
#include <TelepathyQt4/Contact>
#include <TelepathyQt4/ContactManager>
#include <TelepathyQt4/PendingConnection>
#include <TelepathyQt4/PendingContacts>
#include <TelepathyQt4/PendingOperation>
#include <TelepathyQt4/PendingReady>

struct AstralRosterPrivate
{
	AstralAccount *account;
	ContactManager *manager;
	ConnectionPtr conn;
	QHash<QString, QPointer<AstralContact> > contacts;
	QHash<ContactPtr, QPointer<AstralContact> > contactsByPtr;
};

AstralRoster::AstralRoster(AstralAccount *acc, ConnectionPtr conn) : p(new AstralRosterPrivate)
{
	p->conn = conn;
	p->account = acc;
	connect(p->conn->becomeReady(Connection::FeatureRoster),
			SIGNAL(finished(Tp::PendingOperation *)),
			SLOT(onConnectionReady(Tp::PendingOperation *)));
}

AstralRoster::~AstralRoster()
{
}

void AstralRoster::onConnectionReady(Tp::PendingOperation *op)
{
	if (op->isError()) {
		qWarning() << "Connection cannot become ready";
		return;
	}

	PendingReady *pr = qobject_cast<PendingReady *>(op);
	ConnectionPtr conn = ConnectionPtr(qobject_cast<Connection *>(pr->object()));
	p->manager = conn->contactManager();
//	connect(conn->contactManager(),
//			SIGNAL(presencePublicationRequested(const Tp::Contacts &)),
//			SLOT(onPresencePublicationRequested(const Tp::Contacts &)));

	qDebug() << "Connection ready";
//	RosterItem *item;
//	bool exists;
	foreach (const ContactPtr &contact_ptr, p->manager->allKnownContacts())
	{
		AstralContact *contact = new AstralContact(p->account, contact_ptr);
		p->contacts.insert(contact->id(), contact);
		p->account->contactCreated(contact);
//		qDebug() << contact->id() << contact->alias() << contact->groups();
//        exists = false;
//        item = createItemForContact(contact, exists);
//        if (!exists) {
//            connect(item, SIGNAL(changed()), SLOT(updateActions()));
//        }
	}
}

AstralContact *AstralRoster::contact(const QString &id)
{
	return p->contacts.value(id);
}

AstralContact *AstralRoster::contact(const ContactPtr &ptr)
{
	return p->contactsByPtr.value(ptr);
}

void AstralRoster::onContactRetrieved(Tp::PendingOperation *op)
{
}

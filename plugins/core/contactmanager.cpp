#include "contactmanager_p.h"
#include "account.h"
#include "contact_p.h"

void ContactManagerPrivate::setContactManager(const Tp::ContactManagerPtr &newManager)
{
    if (manager == newManager)
        return;

    onStateChanged(Tp::ContactListStateNone);

    manager = newManager;

    if (manager) {
        QObject::connect(manager.data(), &Tp::ContactManager::stateChanged,
                [this] (Tp::ContactListState state) {
            onStateChanged(state);
        });
        onStateChanged(manager->state());
        auto onAllKnownContactsChanged = [this] (const Tp::Contacts &contactsAdded,
                const Tp::Contacts &contactsRemoved) {
            QList<Contact *> added;
            QList<Contact *> removed;
            bool was_added;

            for (Tp::ContactPtr contact : contactsAdded)
                added << ensureContact(contact, &was_added);
            for (Tp::ContactPtr contact : contactsRemoved)
                removed  << contacts.take(contact->id());

            if (!added.isEmpty() || !removed.isEmpty())
                emit q->allKnownContactsChanged(added, removed);

            for (Contact *contact : removed)
                delete contact;

//                contacts[contact->id()]->d->setContact(Tp::ContactPtr());
        };

        QObject::connect(manager.data(), &Tp::ContactManager::allKnownContactsChanged,
                onAllKnownContactsChanged);
    } else {
        onStateChanged(Tp::ContactListStateNone);
    }
}

void ContactManagerPrivate::onStateChanged(Tp::ContactListState newState)
{
    if (state == newState)
        return;
    state = newState;

    if (state == Tp::ContactListStateSuccess) {
        QList<Contact *> added;
        QSet<Contact *> removed = contacts.values().toSet();

        bool wasAdded;
        for (Tp::ContactPtr tpContact : manager->allKnownContacts()) {
            Contact *contact = ensureContact(tpContact, &wasAdded);
            if (wasAdded)
                added << contact;
            else
                removed.remove(contact);
        }
        if (!added.isEmpty() || !removed.isEmpty())
            emit q->allKnownContactsChanged(added, removed.toList());

        for (Contact *contact : removed)
            delete contacts.take(contact->id());
    } else {
        for (Contact *contact : contacts)
            contact->d->setContact(Tp::ContactPtr());
    }
}

Contact *ContactManagerPrivate::ensureContact(const Tp::ContactPtr &tpContact, bool *added)
{
    auto it = contacts.find(tpContact->id());
    if (it != contacts.end()) {
        it.value()->d->setContact(tpContact);
        *added = false;
        return it.value();
    }

    Contact *contact = new Contact(account);
    contact->d->setContact(tpContact);
    contacts.insert(tpContact->id(), contact);

    *added = true;
    return contact;
}

ContactManager::ContactManager(Account *parent) :
    QObject(parent), d(new ContactManagerPrivate(this))
{
    d->account = parent;
}

ContactManager::~ContactManager()
{
}

QList<Contact *> ContactManager::allKnownContacts() const
{
    return d->contacts.values();
}

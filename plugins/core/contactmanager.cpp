#include "contactmanager_p.h"
#include "account.h"
#include "contact_p.h"
#include <QStandardPaths>

#define CONTACTS_CACHE_PATH QLatin1String("qutim/contacts")

inline QDataStream& operator>>(QDataStream &stream, ContactPrivate &priv)
{
    stream >> priv.id
           >> priv.alias
           >> priv.avatar
           >> priv.groups
           >> priv.isAvatarKnown
           >> priv.inList;
    return stream;
}

inline QDataStream& operator<<(QDataStream &stream, const ContactPrivate &priv)
{
    stream << priv.id
           << priv.alias
           << priv.avatar
           << priv.groups
           << priv.isAvatarKnown
           << priv.inList;
    return stream;
}

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

static QString generatePath(Account *account)
{
    return account->uniqueIdentifier() + ".cache";
}

ContactManager::ContactManager(Account *parent) :
    QObject(parent), d(new ContactManagerPrivate(this))
{
    d->account = parent;

    QString cacheFilePath = QStandardPaths::locate(
                QStandardPaths::CacheLocation,
                generatePath(d->account));

    if (cacheFilePath.isEmpty())
        return;

    QFile file(cacheFilePath);
    if (file.open(QFile::ReadOnly)) {
        QDataStream in(&file);
        ContactPrivate priv(NULL);
        priv.presence.setStatus(Presence::Offline, QString(), QString());

        quint32 count = 0;
        in >> count;

        for (quint32 i = 0; i < count; ++i) {
            in >> priv;
            Contact *contact = new Contact(*new ContactPrivate(priv), d->account);
            d->contacts.insert(contact->id(), contact);
        }
    }
}

ContactManager::~ContactManager()
{
    QFileInfo fileInfo(QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
                       + '/'
                       + generatePath(d->account));

    QDir cacheDir = fileInfo.absoluteDir();
    if (!cacheDir.exists()) {
        cacheDir.mkpath(cacheDir.absolutePath());
    }

    QFile file(fileInfo.absoluteFilePath());
    if (file.open(QFile::WriteOnly)) {
        QDataStream out(&file);

        quint32 count = d->contacts.size();
        out << count;

        for (Contact *contact : d->contacts) {
            const ContactPrivate &priv = *contact->d;
            out << priv;
        }
    }
}

QList<Contact *> ContactManager::allKnownContacts() const
{
    return d->contacts.values();
}

#include "contact_p.h"
#include "account.h"
#include <TelepathyQt/AvatarData>

void ContactPrivate::setContact(const Tp::ContactPtr &contact)
{
    auto onPresenceChanged = [this] (const Tp::Presence &tpPresence) {
        Presence newPresence = PresenceData::create(tpPresence);
        if (presence == newPresence)
            return;
        qSwap(presence, newPresence);
        emit q->presenceChanged(presence, newPresence);
    };

    if (contact) {
        if (!id.isEmpty() && contact->id() != id)
            qFatal("ContactPrivate::setContact: is mismatch");

        id = contact->id();

        auto onAliasChanged = [this] (const QString &newAlias) {
            if (alias == newAlias)
                return;
            alias = newAlias;
            q->aliasChanged(alias);
        };
        auto onAvatarChanged = [this] (const Tp::AvatarData &newAvatar) {
            if (avatar == newAvatar.fileName)
                return;
            avatar = newAvatar.fileName;
            q->aliasChanged(avatar);
        };
        auto onGroupAdded = [this] (const QString &group) {
            if (!groups.contains(group))
                return;
            QStringList oldGroups = groups;
            groups << group;
            emit q->addedToGroup(group);
            emit q->groupsChanged(groups, oldGroups);
        };
        auto onGroupRemoved = [this] (const QString &group) {
            if (groups.contains(group))
                return;
            QStringList oldGroups = groups;
            groups.removeOne(group);
            emit q->removedFromGroup(group);
            emit q->groupsChanged(groups, oldGroups);
        };

        QObject::connect(contact.data(), &Tp::Contact::aliasChanged, onAliasChanged);
        QObject::connect(contact.data(), &Tp::Contact::avatarDataChanged, onAvatarChanged);
        QObject::connect(contact.data(), &Tp::Contact::addedToGroup, onGroupAdded);
        QObject::connect(contact.data(), &Tp::Contact::removedFromGroup, onGroupRemoved);
        QObject::connect(contact.data(), &Tp::Contact::presenceChanged, onPresenceChanged);

        onAliasChanged(contact->alias());
        onAvatarChanged(contact->avatarData());
        onPresenceChanged(contact->presence());

        QSet<QString> newGroups = contact->groups().toSet();
        QSet<QString> oldGroups = groups.toSet();

        for (auto group : QSet<QString>(newGroups).subtract(oldGroups))
            onGroupAdded(group);
        for (auto group : oldGroups.subtract(newGroups))
            onGroupRemoved(group);
    } else {
        onPresenceChanged(Tp::Presence::offline());
    }
}

Contact::Contact(Account *parent) :
    QObject(parent), d(new ContactPrivate(this))
{
    d->account = parent;
}

Contact::~Contact()
{
}

Account *Contact::account() const
{
    return d->account;
}

QString Contact::id() const
{
    Q_ASSERT(!d->id.isEmpty());
    return d->id;
}

QString Contact::alias() const
{
    return d->alias;
}

QStringList Contact::groups() const
{
    return d->groups;
}

Presence Contact::presence() const
{
    return d->presence;
}

bool Contact::isInList() const
{
    return d->inList;
}

QString Contact::avatar() const
{
    return d->avatar;
}

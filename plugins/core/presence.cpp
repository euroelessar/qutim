#include "presence_p.h"

static const QSharedDataPointer<PresenceData> emptyPresence(new PresenceData);

Presence PresenceData::create(const Tp::Presence &other)
{
    Presence presence;
    presence.d->presence = other;
    return presence;
}

Presence::Presence()
    : d(emptyPresence)
{
}

Presence::Presence(Presence::Type type, const QString &status, const QString &statusMessage)
    : d(emptyPresence)
{
    setStatus(type, status, statusMessage);
}

Presence::Presence(const Presence &other) : d(other.d)
{
}

Presence::~Presence()
{
}

Presence &Presence::operator =(const Presence &other)
{
    d = other.d;
    return *this;
}

bool Presence::operator ==(const Presence &other) const
{
    return d->presence == other.d->presence;
}

bool Presence::operator !=(const Presence &other) const
{
    return d->presence != other.d->presence;
}

bool Presence::isOffline() const
{
    switch (type()) {
    case Offline:
    case Unknown:
    case Error:
        return true;
    default:
        return false;
    }
}

bool Presence::isOnline() const
{
    return !isOffline();
}

bool Presence::isError() const
{
    return type() == Error;
}

Presence::Type Presence::type() const
{
    return static_cast<Type>(d->presence.type());
}

QString Presence::status() const
{
    return d->presence.status();
}

QString Presence::statusMessage() const
{
    return d->presence.statusMessage();
}

void Presence::setStatus(Presence::Type type, const QString &status, const QString &statusMessage)
{
    d->presence.setStatus(static_cast<Tp::ConnectionPresenceType>(type), status, statusMessage);
}

void Presence::setStatusMessage(const QString &statusMessage)
{
    d->presence.setStatusMessage(statusMessage);
}

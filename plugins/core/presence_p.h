#ifndef PRESENCE_P_H
#define PRESENCE_P_H

#include "presence.h"
#include <TelepathyQt/Presence>

class PresenceData : public QSharedData
{
public:
    PresenceData()
    {
    }

    PresenceData(const PresenceData &other)
        : QSharedData(other), presence(other.presence)
    {
    }

    static Presence create(const Tp::Presence &other);

    Tp::Presence presence;
};

#endif // PRESENCE_P_H

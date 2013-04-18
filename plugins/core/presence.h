#ifndef PRESENCE_H
#define PRESENCE_H

#include <QSharedData>
#include <QMetaType>
#include <QString>

class PresenceData;

class Presence
{
public:
    enum Type {
        Invalid,
        Offline,
        Available,
        Away,
        ExtendedAway,
        Invisible,
        Busy,
        Unknown,
        Error
    };

    Presence();
    Presence(Type type, const QString &status, const QString &statusMessage = QString());
    Presence(const Presence &other);
    ~Presence();

    Presence &operator =(const Presence &other);
    bool operator==(const Presence &other) const;
    bool operator!=(const Presence &other) const;

    bool isOffline() const;
    bool isOnline() const;
    bool isError() const;

    Type type() const;
    QString status() const;
    QString statusMessage() const;
    void setStatus(Type type, const QString &status, const QString &statusMessage = QString());
    void setStatusMessage(const QString &statusMessage);

private:
    friend class PresenceData;
    QSharedDataPointer<PresenceData> d;
};

Q_DECLARE_METATYPE(Presence)

#endif // PRESENCE_H

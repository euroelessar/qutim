#ifndef ROSTER_H
#define ROSTER_H

#include <QMultiMap>

#include "mrimaccount.h"
#include "protoutils.h"

#include "mrimpacket.h"

class QParseMultiMap : public QMultiMap<QChar,QVariant>
{
public:
    template <class T>
    T get(QChar key, quint32 index);
    
    inline quint32 getUint(quint32 index);
    inline QString getString(quint32 index, bool unicode = false);
};

template <class T>
T QParseMultiMap::get(QChar key, quint32 index)
{
    return values(key)[index].value<T>();
}

inline quint32 QParseMultiMap::getUint(quint32 index)
{
    return get<quint32>('u',index);
}
 
inline QString QParseMultiMap::getString(quint32 index, bool unicode)
{
    return get<LPString>('s',index).toString(unicode);
}

class Roster : public QObject, public PacketHandler
{
    Q_OBJECT
public:
    Roster(MrimAccount* acc);
    virtual ~Roster();
    virtual QList<quint32> handledTypes();
    virtual bool handlePacket(MrimPacket& packet);
    
protected:
    bool parseList(MrimPacket& packet);    
    bool parseGroups(MrimPacket& packet, quint32 count, const QString& mask);
    bool parseContacts(MrimPacket& packet, const QString& mask);
    
    QParseMultiMap parseByMask(MrimPacket& packet, const QString& mask);
    
private:
    Q_DISABLE_COPY(Roster);
    QScopedPointer<struct RosterPrivate> p;
};

#endif // ROSTER_H

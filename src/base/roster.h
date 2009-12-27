/****************************************************************************
 *  roster.h
 *
 *  Copyright (c) 2009 by Rusanov Peter <peter.rusanov@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef ROSTER_H
#define ROSTER_H

#include <QMultiMap>
#include <QVariant>

#include "protoutils.h"

#include "mrimpacket.h"

class RosterParseMultiMap : public QMultiMap<QChar,QVariant>
{
public:
    template <class T>
    T get(QChar key, quint32 index);
    
    inline quint32 getUint(quint32 index);
    inline QString getString(quint32 index, bool unicode = false);
};

template <class T>
T RosterParseMultiMap::get(QChar key, quint32 index)
{
    QVariantList vals = values(key);
    return vals.value(vals.count() - index - 1).value<T>();
}

inline quint32 RosterParseMultiMap::getUint(quint32 index)
{
    return get<quint32>('u',index);
}
 
inline QString RosterParseMultiMap::getString(quint32 index, bool unicode)
{
    return get<LPString>('s',index).toString(unicode);
}

class Roster : public QObject, public PacketHandler
{
    Q_OBJECT
public:
    Roster(class MrimAccount* acc);
    virtual ~Roster();
    virtual QList<quint32> handledTypes();
    virtual bool handlePacket(MrimPacket& packet);
    QString groupName(quint32 groupId) const;

protected:
    void addToList(class MrimContact *cnt);
    bool parseList(MrimPacket& packet);    
    bool parseGroups(MrimPacket& packet, quint32 count, const QString& mask);
    bool parseContacts(MrimPacket& packet, const QString& mask);
    
    RosterParseMultiMap parseByMask(MrimPacket& packet, const QString& mask);

private:
    Q_DISABLE_COPY(Roster);
    QScopedPointer<struct RosterPrivate> p;
};

#endif // ROSTER_H


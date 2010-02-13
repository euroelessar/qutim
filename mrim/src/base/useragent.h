/****************************************************************************
 *  useragent.h
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

#ifndef USERAGENT_H
#define USERAGENT_H

#include <QObject>
#include <QString>
#include <QDebug>

class QIcon;

class UserAgent : public QObject
{
    Q_OBJECT

public:
    UserAgent();
    UserAgent( const QString &clientID, const QString &versionStr, const QString &buildVer,
               quint8 protoMajorVer, quint8 protoMinorVer );
    virtual ~UserAgent();

    void clear();
    bool parse(const QString &userAgentStr);
    QString toString() const;
    QString toReadable() const;
    bool isEmpty() const;
    QIcon icon() const;
    void set(const UserAgent& newAgent);

signals:
    void changed();

private:
    //Client ID
    QString m_clientID;
    //Client version
    QString m_versionStr;
    QString m_buildVer;
    //Protocol version
    quint8 m_protoMajorVer;
    quint8 m_protoMinorVer;
};

QDebug operator<<(QDebug dbg, const UserAgent &u);

#endif // USERAGENT_H

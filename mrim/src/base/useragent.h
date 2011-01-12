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
#include <QSharedData>
#include <qutim/extensionicon.h>

class QIcon;

class UserAgentData : public QSharedData
{
public:
	UserAgentData() : protoMajorVer(0), protoMinorVer(0) {}
	UserAgentData(const UserAgentData &o)
		: QSharedData(o), clientID(o.clientID), versionStr(o.versionStr), buildVer(o.buildVer),
		protoMajorVer(o.protoMajorVer), protoMinorVer(o.protoMinorVer) {}
    //Client ID
    QString clientID;
    //Client version
    QString versionStr;
    QString buildVer;
    //Protocol version
    quint8 protoMajorVer;
    quint8 protoMinorVer;
};

class MrimUserAgent
{
public:
    MrimUserAgent();
    MrimUserAgent( const QString &clientID, const QString &versionStr, const QString &buildVer,
               quint8 protoMajorVer, quint8 protoMinorVer );
    MrimUserAgent(const MrimUserAgent &o);
	MrimUserAgent &operator =(const MrimUserAgent &o);
    virtual ~MrimUserAgent();

    void clear();
    bool parse(const QString &userAgentStr);
    QString toString() const;
    QString toReadable() const;
    bool isEmpty() const;
    qutim_sdk_0_3::ExtensionIcon icon() const;
private:
	QSharedDataPointer<UserAgentData> d;
};

QDebug operator<<(QDebug dbg, const MrimUserAgent &u);

#endif // USERAGENT_H

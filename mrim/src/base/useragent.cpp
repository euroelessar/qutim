/****************************************************************************
 *  useragent.cpp
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

#include <QRegExp>
#include <QIcon>
#include <QStringList>

#include <qutim/icon.h>

#include "useragent.h"

using namespace qutim_sdk_0_3;

MrimUserAgent::MrimUserAgent( const QString &clientID, const QString &versionStr, const QString &buildVer,
                      quint8 protoMajorVer, quint8 protoMinorVer )
						  : d(new UserAgentData)
{
	d->clientID = clientID;
	d->versionStr = versionStr;
	d->buildVer = buildVer;
	d->protoMajorVer = protoMajorVer;
	d->protoMinorVer = protoMinorVer;
}

MrimUserAgent::MrimUserAgent(const MrimUserAgent &o) : d(o.d)
{
}

MrimUserAgent::MrimUserAgent() : d(new UserAgentData)
{
    clear();
}

MrimUserAgent &MrimUserAgent::operator =(const MrimUserAgent &o)
{
	d = o.d;
	return *this;
}

MrimUserAgent::~MrimUserAgent()
{
}

void MrimUserAgent::clear()
{
   d->clientID.clear();
   d->versionStr.clear();
   d->buildVer.clear();
   d->protoMajorVer = 0;
   d->protoMinorVer = 0;
}

bool MrimUserAgent::parse(const QString &userAgentStr)
{
    clear();

    if (userAgentStr.isEmpty()) return false;

    QRegExp rx("((\\w+)=\\\"([\\w \\t\\.]+)\\\"*)+");

	qint32 offset = 0;
    QString currParam;

    while ((offset = rx.indexIn(userAgentStr, offset)) != -1)
    {
//        qDebug()<<rx.capturedTexts();
        currParam = rx.cap(2);

        if (currParam == "client")
            d->clientID = rx.cap(3);
        else if (currParam == "version")
            d->versionStr = rx.cap(3);
        else if (currParam == "build")
            d->buildVer = rx.cap(3);
        else if (currParam == "protocol")
        {
            QRegExp verRx("(\\d+)\\.(\\d+)");

            if (verRx.exactMatch(rx.cap(3)))
            {
                d->protoMajorVer = verRx.cap(1).toUInt();
                d->protoMinorVer = verRx.cap(2).toUInt();
            }
        }
        offset += rx.matchedLength();
    }

    return true;
}

QString MrimUserAgent::toString() const
{
//    QString protoVerStr = QString("%1.%2").arg(d->protoMajorVer).arg(d->protoMinorVer);
    return QString("client=\"%1\" version=\"%2\" build=\"%3\"").arg(d->clientID).arg(d->versionStr).arg(d->buildVer);
}

QString MrimUserAgent::toReadable() const
{
    if (isEmpty())
        return QString("-");

    QString userAgentFmt("%1 %2 %3");
    return userAgentFmt.arg(d->clientID).arg(d->versionStr).arg(d->buildVer);
}

bool MrimUserAgent::isEmpty() const
{
    return (d->clientID.isEmpty());
}

ExtensionIcon MrimUserAgent::icon() const
{
    QString iconName = d->clientID.toLower();

    if(iconName.isEmpty())
            return QIcon();
    else if(iconName.startsWith("qutim"))
            iconName="qutim";
    else if(iconName.startsWith("qip infium"))
            iconName="qipinf";
    else if(iconName.startsWith("magent"))
            iconName="magent";
    else if(iconName.startsWith("mchat"))
            iconName="mchat";
    else if(iconName.startsWith("imadering"))
            iconName="imadering";
    else if(iconName.startsWith("mraqt"))
            iconName="mraqt";
    else if(iconName.startsWith("miranda"))
            iconName="miranda";
    else if(iconName.startsWith("psi"))
            iconName="psi";
    else if(iconName.startsWith("pidgin"))
            iconName="pidgin";
    else if(iconName.startsWith("mdc"))
            iconName="mdc";
	else
		iconName = iconName.replace(QLatin1String(" "), QLatin1String("-"));
	return ExtensionIcon(iconName);
}

QDebug operator<<(QDebug dbg, const MrimUserAgent &u)
{
	dbg.nospace() << "UserAgent (" << u.toString() << ")";
	return dbg.space();
}

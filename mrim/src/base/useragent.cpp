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

UserAgent::UserAgent( const QString &clientID, const QString &versionStr, const QString &buildVer,
                      quint8 protoMajorVer, quint8 protoMinorVer )
: m_clientID(clientID), m_versionStr(versionStr), m_buildVer(buildVer), m_protoMajorVer(protoMajorVer), m_protoMinorVer(protoMinorVer)
{ }

UserAgent::UserAgent()
{
    clear();
}

UserAgent::~UserAgent()
{ }

void UserAgent::clear()
{
   m_clientID.clear();
   m_versionStr.clear();
   m_buildVer.clear();
   m_protoMajorVer = 0;
   m_protoMinorVer = 0;
}

bool UserAgent::parse( const QString &userAgentStr)
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
            m_clientID = rx.cap(3);
        else if (currParam == "version")
            m_versionStr = rx.cap(3);
        else if (currParam == "build")
            m_buildVer = rx.cap(3);
        else if (currParam == "protocol")
        {
            QRegExp verRx("(\\d+)\\.(\\d+)");

            if (verRx.exactMatch(rx.cap(3)))
            {
                m_protoMajorVer = verRx.cap(1).toUInt();
                m_protoMinorVer = verRx.cap(2).toUInt();
            }
        }
        offset += rx.matchedLength();
    }

    return true;
}

QString UserAgent::toString() const
{
//    QString protoVerStr = QString("%1.%2").arg(m_protoMajorVer).arg(m_protoMinorVer);
    return QString("client=\"%1\" version=\"%2\" build=\"%3\"").arg(m_clientID).arg(m_versionStr).arg(m_buildVer);
}

QString UserAgent::toReadable() const
{
    if (isEmpty())
        return QString("-");

    QString userAgentFmt("%1 %2 %3");
    return userAgentFmt.arg(m_clientID).arg(m_versionStr).arg(m_buildVer);
}

bool UserAgent::isEmpty() const
{
    return (m_clientID.isEmpty());
}

QIcon UserAgent::icon() const
{
    QString icon_name = m_clientID.toLower();

    if(icon_name.isEmpty())
            return QIcon();
    else if(icon_name.startsWith("qutim"))
            icon_name="qutim";
    else if(icon_name.startsWith("qip infium"))
            icon_name="qipinf";
    else if(icon_name.startsWith("magent"))
            icon_name="magent";
    else if(icon_name.startsWith("mchat"))
            icon_name="mchat";
    else if(icon_name.startsWith("imadering"))
            icon_name="imadering";
    else if(icon_name.startsWith("mraqt"))
            icon_name="mraqt";
    else if(icon_name.startsWith("miranda"))
            icon_name="miranda";
    else if(icon_name.startsWith("psi"))
            icon_name="psi";
    else if(icon_name.startsWith("pidgin"))
            icon_name="pidgin";
    else if(icon_name.startsWith("mdc"))
            icon_name="mdc";

    QIcon icon = Icon( icon_name );

    if( icon.isNull() )
            icon = Icon( "unknown" );
    return icon;
}

void UserAgent::set(const UserAgent& newAgent)
{
    m_clientID = newAgent.m_clientID;
    m_versionStr = newAgent.m_versionStr;
    m_buildVer = newAgent.m_buildVer;
    m_protoMajorVer = newAgent.m_protoMajorVer;
    m_protoMinorVer = newAgent.m_protoMinorVer;
    emit changed();
}

QDebug operator<<(QDebug dbg, const UserAgent &u)
{
	dbg.nospace() << "UserAgent (" << u.toString() << ")";
	return dbg.space();
}

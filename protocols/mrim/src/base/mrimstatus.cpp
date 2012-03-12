/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#include "mrimstatus.h"
#include "proto.h"
#include "useragent.h"
#include "mrimcontact.h"
#include <qutim/debug.h>
#include <qutim/tooltip.h>

MrimStatus::MrimStatus(Type type) : Status(Status::instance(type, "mrim"))
{
}

MrimStatus::MrimStatus(const QString &uri, const QString &title, const QString &desc)
	: Status(MrimStatus::fromString(uri, title, desc))
{
	debug() << uri << type();
}

MrimStatus::MrimStatus(const MrimStatus &status) : Status(status)
{
}

MrimStatus::MrimStatus(const Status &status) : Status(Status::instance(status.type(), "mrim"))
{
	setType(status.type());
	setSubtype(status.subtype());
	setText(status.text());
}

MrimStatus &MrimStatus::operator =(const MrimStatus &o)
{
	return static_cast<MrimStatus&>(Status::operator =(o));
}

MrimStatus &MrimStatus::operator =(const Status &o)
{
	return static_cast<MrimStatus&>(Status::operator =(MrimStatus(o)));
}

quint32 MrimStatus::mrimType() const
{
	if (subtype())
		return STATUS_USER_DEFINED;
	switch (type()) {
    case Status::Offline:
		return STATUS_OFFLINE;
    case Status::Online:
		return STATUS_ONLINE;
    case Status::Away:
		return STATUS_AWAY;
    case Status::Invisible:
		return STATUS_FLAG_INVISIBLE;
	default:
		return STATUS_USER_DEFINED;
	}
}

void MrimStatus::setUserAgent(const MrimUserAgent &info)
{
	QVariantHash clientInfo;
	clientInfo.insert("id", "client");
	clientInfo.insert("title", QObject::tr("Possible client"));
	clientInfo.insert("icon", QVariant::fromValue(info.icon()));
	clientInfo.insert("description", info.toReadable());
	clientInfo.insert("showInTooltip", true);
	clientInfo.insert("priorityInContactList", 85);
	clientInfo.insert("priorityInTooltip", 25);
	clientInfo.insert("iconPosition", qVariantFromValue(ToolTipEvent::IconBeforeDescription));
	setExtendedInfo("client", clientInfo);
}

void MrimStatus::setFlags(quint32 serverFlags)
{
	if (serverFlags & CONTACT_INTFLAG_NOT_AUTHORIZED) {
		QVariantHash clientInfo;
		clientInfo.insert("id", "authorization");
		clientInfo.insert("title", QObject::tr("Not authorized"));
		clientInfo.insert("icon", QVariant::fromValue(ExtensionIcon("dialog-warning")));
		clientInfo.insert("description", QString());
		clientInfo.insert("showInTooltip", true);
		clientInfo.insert("priorityInContactList", 80);
		clientInfo.insert("priorityInTooltip", 30);
		clientInfo.insert("iconPosition", qVariantFromValue(ToolTipEvent::IconBeforeDescription));
		setExtendedInfo("authorization", clientInfo);
	}
}

QString MrimStatus::toString() const
{
    QString statusStr = "status_";

    switch (type()) {
    case Status::Offline:
        statusStr += "offline";
        break;
    case Status::Online:
        statusStr += (subtype() == 0) ? "online" : QString::number(subtype());
        break;
    case Status::Away:
        statusStr += "away";
        break;
    case Status::NA:
        statusStr += "7"; //=where am i?
        break;
    case Status::DND:
        statusStr += "19"; //=meeting
        break;
    case Status::FreeChat:
        statusStr += "chat";
        break;
    case Status::Invisible:
        statusStr += "invisible";
        break;
    default:
        statusStr.clear();
        break;
    }
    return statusStr;
}

Status MrimStatus::fromString(const QString &uri, const QString &title,
                              const QString &desc)
{
    Status::Type type = Status::Offline;
    quint32 subtype = 0;
    QString statusStr = uri.toLower();
    statusStr.remove("status_");

    if (statusStr == "offline" || statusStr == "undeterminated") {
        type = Status::Offline;
    } else if (statusStr == "online" || statusStr == "status_1") {
        type = Status::Online;
    } else if (statusStr == "away" || statusStr == "status_2") {
        type = Status::Away;
    } else if (statusStr == "chat") {
        type = Status::FreeChat;
    } else if (statusStr == "invisible" || statusStr == "status_3") {
        type = Status::Invisible;
    } else if (statusStr == "status_7") {//=where am i?
        type = Status::NA;
    } else if (statusStr == "status_19") {//=meeting
        type = Status::DND;
    } else {        
        bool ok = false;
        subtype = statusStr.toUInt(&ok);
        type = ok ? Status::Online : Status::Offline;
    }

    Status status(type);
    status.setSubtype(subtype);
    QString statusText = title;

    if (!desc.isEmpty()) {
        statusText += " - " + desc;
    }
    status.setText(statusText);
    return status;
}

LocalizedString MrimStatus::defaultName(const Status &status)
{    
    switch (status.subtype())
    {
    case 0:
        return status.name();
    case 4:
        return QT_TRANSLATE_NOOP("Status","Sick");
    case 5:
        return QT_TRANSLATE_NOOP("Status","At home");
    case 6:
        return QT_TRANSLATE_NOOP("Status","Lunch");
    case 7:
        return QT_TRANSLATE_NOOP("Status","Where am I?");
    case 8:
        return QT_TRANSLATE_NOOP("Status","WC");
    case 9:
        return QT_TRANSLATE_NOOP("Status","Cooking");
    case 10:
        return QT_TRANSLATE_NOOP("Status","Walking");
    case 11:
        return QT_TRANSLATE_NOOP("Status","I'm an alien!");
    case 12:
        return QT_TRANSLATE_NOOP("Status","I'm a shrimp!");
    case 13:
        return QT_TRANSLATE_NOOP("Status","I'm lost :(");
    case 14:
        return QT_TRANSLATE_NOOP("Status","Crazy %)");
    case 15:
        return QT_TRANSLATE_NOOP("Status","Duck");
    case 16:
        return QT_TRANSLATE_NOOP("Status","Playing");
    case 17:
        return QT_TRANSLATE_NOOP("Status","Smoke");
    case 18:
        return QT_TRANSLATE_NOOP("Status","At work");
    case 19:
        return QT_TRANSLATE_NOOP("Status","Meeting");
    case 20:
        return QT_TRANSLATE_NOOP("Status","Beer");
    case 21:
        return QT_TRANSLATE_NOOP("Status","Coffee");
    case 22:
        return QT_TRANSLATE_NOOP("Status","Shovel");
    case 23:
        return QT_TRANSLATE_NOOP("Status","Sleeping");
    case 24:
        return QT_TRANSLATE_NOOP("Status","On the phone");
    case 26:
        return QT_TRANSLATE_NOOP("Status","University");
    case 27:
        return QT_TRANSLATE_NOOP("Status","School");
    case 28:
        return QT_TRANSLATE_NOOP("Status","Wrong number!");
    case 29:
        return QT_TRANSLATE_NOOP("Status","LOL");
    case 30:
        return QT_TRANSLATE_NOOP("Status","Tongue");
    case 32:
        return QT_TRANSLATE_NOOP("Status","Smiley");
    case 33:
        return QT_TRANSLATE_NOOP("Status","Hippy");
    case 34:
        return QT_TRANSLATE_NOOP("Status","Depression");
    case 35:
        return QT_TRANSLATE_NOOP("Status","Crying");
    case 36:
        return QT_TRANSLATE_NOOP("Status","Surprised");
    case 37:
        return QT_TRANSLATE_NOOP("Status","Angry");
    case 38:
        return QT_TRANSLATE_NOOP("Status","Evil");
    case 39:
        return QT_TRANSLATE_NOOP("Status","Ass");
    case 40:
        return QT_TRANSLATE_NOOP("Status","Heart");
    case 41:
        return QT_TRANSLATE_NOOP("Status","Crescent");
    case 42:
        return QT_TRANSLATE_NOOP("Status","Coool!");
    case 43:
        return QT_TRANSLATE_NOOP("Status","Horns");
    case 44:
        return QT_TRANSLATE_NOOP("Status","Figa");
    case 45:
        return QT_TRANSLATE_NOOP("Status","F*ck you!");
    case 46:
        return QT_TRANSLATE_NOOP("Status","Skull");
    case 47:
        return QT_TRANSLATE_NOOP("Status","Rocket");
    case 48:
        return QT_TRANSLATE_NOOP("Status","Ktulhu");
    case 49:
        return QT_TRANSLATE_NOOP("Status","Goat");
    case 50:
        return QT_TRANSLATE_NOOP("Status","Must die!");
    case 51:
        return QT_TRANSLATE_NOOP("Status","Squirrel");
    case 52:
        return QT_TRANSLATE_NOOP("Status","Party!");
    case 53:
        return QT_TRANSLATE_NOOP("Status","Music");
    default:
        return QT_TRANSLATE_NOOP("Status","?");
    }
}


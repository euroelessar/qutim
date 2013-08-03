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

#include "status.h"
#include "dynamicpropertydata_p.h"
#include "icon.h"
#include <QDebug>

typedef QHash<QString, QVariantHash> ExtendedStatus;
Q_DECLARE_METATYPE(ExtendedStatus)

namespace qutim_sdk_0_3
{
struct StatusHashKey
{
	const char *name;
	int type;
	int subtype;

	bool operator ==(const StatusHashKey &o) const
	{
		// Check name only if both type and subtype equals
		return o.type == type
				&& o.subtype == subtype
				&& (o.name == name || !qstrcmp(o.name, name));
	}
};

uint qHash(const qutim_sdk_0_3::StatusHashKey &value, uint seed = 0) Q_DECL_NOTHROW
{
	// Simple hash algorithm
	const uint p = 373;
	uint h = seed;
	const char *c = value.name;
	while (*c)
		h = h * p + *(c++);
	h = h * p + value.type;
	h = h * p + value.subtype;
	return h;
}

class StatusPrivate : public DynamicPropertyData
{
public:
	StatusPrivate() : type(Status::Offline), subtype(0), changeReason(Status::ByUser) {}
	StatusPrivate(const StatusPrivate &o) :
		DynamicPropertyData(o), text(o.text), name(o.name),
		icon(o.icon), type(o.type), subtype(o.subtype),
	    changeReason(o.changeReason), extStatuses(o.extStatuses) {}
	~StatusPrivate() {}
	QString text;
	LocalizedString name;
	QIcon icon;
	Status::Type type;
	int subtype;
	Status::ChangeReason changeReason;
	QHash<QString, QVariantHash> extStatuses;

	QVariant getText() const { return text; }
	void setText(const QVariant &val) { text = val.toString(); }
	QVariant getName() const { return name.toString(); }
	void setName(const QVariant &val) { name = LocalizedString(val.toString().toUtf8()); }
	QVariant getIcon() const { return icon; }
	void setIcon(const QVariant &val) { icon = val.value<QIcon>(); }
	QVariant getType() const { return type; }
	void setType(const QVariant &val) { type = static_cast<Status::Type>(val.toInt()); }
	QVariant getSubtype() const { return type; }
	void setSubtype(const QVariant &val) { subtype = val.toInt(); }
	QVariant getChangeReason() const { return QVariant::fromValue(changeReason); }
	void setChangeReason(const QVariant &val) { changeReason = val.value<Status::ChangeReason>(); }
	QVariant getExtendedStatuses() const { return QVariant::fromValue(extStatuses); }
    void setExtendedStatuses(const QVariant &val) { extStatuses = val.value<ExtendedStatus>(); }

	void generateName();
};

class StatusPrivateList : public QVector<QSharedDataPointer<StatusPrivate> >
{
public:
    StatusPrivateList()
    {
        reserve(Status::Offline - Status::Connecting + 1);
        for (int i = Status::Connecting; i <= Status::Offline; i++) {
            Status::Type type = static_cast<Status::Type>(i);
            QSharedDataPointer<StatusPrivate> d(new StatusPrivate);
            d->type = type;
            d->generateName();
            d->icon = Status::createIcon(type);
            append(d);
        }
    }

};

Q_GLOBAL_STATIC(StatusPrivateList, statusList)

static QSharedDataPointer<StatusPrivate> get_status_private(Status::Type type)
{
	int index = type - Status::Connecting;
	StatusPrivateList &list = *statusList();
	Q_ASSERT(index >= 0 && index < list.size());
	return list.at(index);
}

namespace CompiledProperty
{
static QList<QByteArray> names = QList<QByteArray>()
<< "text"
<< "name"
<< "icon"
<< "type"
<< "subtype"
<< "changeReason"
<< "extendedStatuses";
static QList<Getter> getters   = QList<Getter>()
<< static_cast<Getter>(&StatusPrivate::getText)
<< static_cast<Getter>(&StatusPrivate::getName)
<< static_cast<Getter>(&StatusPrivate::getIcon)
<< static_cast<Getter>(&StatusPrivate::getType)
<< static_cast<Getter>(&StatusPrivate::getSubtype)
<< static_cast<Getter>(&StatusPrivate::getChangeReason)
<< static_cast<Getter>(&StatusPrivate::getExtendedStatuses);
static QList<Setter> setters   = QList<Setter>()
<< static_cast<Setter>(&StatusPrivate::setText)
<< static_cast<Setter>(&StatusPrivate::setName)
<< static_cast<Setter>(&StatusPrivate::setIcon)
<< static_cast<Setter>(&StatusPrivate::setType)
<< static_cast<Setter>(&StatusPrivate::setSubtype)
<< static_cast<Setter>(&StatusPrivate::setChangeReason)
<< static_cast<Setter>(&StatusPrivate::setExtendedStatuses);
}

void StatusPrivate::generateName()
{
	switch (type) {
	case Status::Online:
		name = QT_TRANSLATE_NOOP("Status", "Online");
		break;
	case Status::FreeChat:
		name = QT_TRANSLATE_NOOP("Status", "Free for chat");
		break;
	case Status::Away:
		name = QT_TRANSLATE_NOOP("Status", "Away");
		break;
	case Status::NA:
		name = QT_TRANSLATE_NOOP("Status", "Not available");
		break;
	case Status::DND:
		name = QT_TRANSLATE_NOOP("Status", "Do not disturb");
		break;
	case Status::Invisible:
		name = QT_TRANSLATE_NOOP("Status", "Invisible");
		break;
	case Status::Offline:
		name = QT_TRANSLATE_NOOP("Status", "Offline");
		break;
	case Status::Connecting:
		name = QT_TRANSLATE_NOOP("Status", "Connecting");
		break;
	default:
		name = QT_TRANSLATE_NOOP("Status", "Unknown");
		break;
	}
}

Status::Status(Type type) : d(get_status_private(type))
{
}

Status::Status(const Status &other) : d(other.d)
{
}

Status &Status::operator =(const Status &other)
{
	d = other.d;
	return *this;
}

Status &Status::operator =(Status::Type type)
{
	d = get_status_private(type);
	return *this;
}

Status::~Status()
{
}

bool Status::operator ==(Type type) const
{
	return d->type == type;
}

QString Status::text() const
{
	return d->text;
}

void Status::setText(const QString &text)
{
	d->text = text;
}

LocalizedString Status::name() const
{
	return d->name;
}

void Status::setName(const LocalizedString &name)
{
	d->name = name;
}

QIcon Status::icon() const
{
	return d->icon;
}

void Status::setIcon(const QIcon &icon)
{
	d->icon = icon;
}

Status::Type Status::type() const
{
	return d->type;
}

void Status::setType(Status::Type type)
{
	d->type = type;
	d->subtype = 0;
	QSharedDataPointer<StatusPrivate> p = get_status_private(type);
	d->name = p->name;
	d->icon = p->icon;
}

int Status::subtype() const
{
	return d->subtype;
}

void Status::setSubtype(int stype)
{
	d->subtype = stype;
}

Status::ChangeReason Status::changeReason() const
{
	return d->changeReason;
}

void Status::setChangeReason(Status::ChangeReason reason)
{
	d->changeReason = reason;
}

QVariant Status::property(const char *name, const QVariant &def) const
{
	return d->property(name, def, CompiledProperty::names, CompiledProperty::getters);
}

void Status::setProperty(const char *name, const QVariant &value)
{
	d->setProperty(name, value, CompiledProperty::names, CompiledProperty::setters);
}

void Status::initIcon(const QString &protocol)
{
	d->icon = createIcon(d->type, protocol);
}

QIcon Status::createIcon(Type type, const QString &protocol)
{
	return Icon(iconName(type,protocol));
}

QString Status::iconName(Type type, const QString &protocol)
{
	QString name = QLatin1String("user-");

	switch (type) {
	case Status::Online:
		name += QLatin1String("online");
		break;
	case Status::FreeChat:
		name += QLatin1String("online-chat");
		break;
	case Status::Away:
		name += QLatin1String("away");
		break;
	case Status::NA:
		name += QLatin1String("away-extended");
		break;
	case Status::DND:
		name += QLatin1String("busy");
		break;
	case Status::Invisible:
		name += QLatin1String("invisible");
		break;
	case Status::Offline:
		name += QLatin1String("offline");
		break;
	case Status::Connecting:
		name += QLatin1String("network-connect");
		break;
	default:
		return QString();
	}
	if (!protocol.isEmpty()) {
		name += QLatin1Char('-');
		name += protocol;
	}
	return name;
}


typedef QHash<StatusHashKey, Status> StatusHash;
Q_GLOBAL_STATIC(StatusHash, statusHash)

Status Status::instance(Type type, const char *proto, int subtype)
{
	StatusHashKey key = { proto, int(type), subtype };
	return statusHash()->value(key);
}

bool Status::remember(const Status &status, const char *proto)
{
	StatusHashKey key = { proto, int(status.type()), status.subtype() };
	if (statusHash()->contains(key))
		return false;
	key.name = qstrdup(key.name);
	statusHash()->insert(key, status);
	return true;
}

Status Status::createConnecting(const Status &status, const char *proto)
{
	Status connecting = instance(Status::Connecting, proto);
	if (connecting != Status::Connecting) {
		connecting = Status(Status::Connecting);
		connecting.initIcon(QLatin1String(proto));
	}
	connecting.setProperty("connectingGoal", QVariant::fromValue(status));
	return connecting;
}

Status Status::connectingGoal(const Status &status)
{
	if (status != Status::Connecting)
		return Status(Status::Offline);
	return status.property("connectingGoal", Status(Status::Online));
}

Status Status::connectingGoal() const
{
	return connectingGoal(*this);
}

void Status::setExtendedInfo(const QString &name, const QVariantHash &status)
{
	d->extStatuses.insert(name,status);
}

void Status::removeExtendedInfo(const QString &name)
{
	d->extStatuses.remove(name);
}

QVariantHash Status::extendedInfo(const QString &name) const
{
	return d->extStatuses.value(name);
}

QHash<QString, QVariantHash> Status::extendedInfos() const
{
	return d->extStatuses;
}

void Status::setExtendedInfos(const QHash<QString, QVariantHash> &extInfos)
{
	d->extStatuses = extInfos;
}

class ExtendedStatusesEventPrivate
{
public:
	QHash<QString, QVariantHash> infos;
};

ExtendedInfosEvent::ExtendedInfosEvent() :
	QEvent(eventType()), d(new ExtendedStatusesEventPrivate)
{
}

ExtendedInfosEvent::~ExtendedInfosEvent()
{
}

void ExtendedInfosEvent::addInfo(const QString &name, const QVariantHash &info)
{
	d->infos.insert(name, info);
}

QHash<QString, QVariantHash> ExtendedInfosEvent::infos()
{
	return d->infos;
}

QEvent::Type ExtendedInfosEvent::eventType()
{
	static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 106));
	return type;
}
}

QDebug operator<<(QDebug dbg, qutim_sdk_0_3::Status::Type status)
{
	switch (status) {
	case qutim_sdk_0_3::Status::Online:
		return dbg << "Status::Online";
	case qutim_sdk_0_3::Status::FreeChat:
		return dbg << "Status::FreeChat";
	case qutim_sdk_0_3::Status::Away:
		return dbg << "Status::Away";
	case qutim_sdk_0_3::Status::NA:
		return dbg << "Status::NA";
	case qutim_sdk_0_3::Status::DND:
		return dbg << "Status::DND";
	case qutim_sdk_0_3::Status::Invisible:
		return dbg << "Status::Invisible";
	case qutim_sdk_0_3::Status::Offline:
		return dbg << "Status::Offline";
	default:
		return dbg << "Status::Unknown";
	}
}

QDebug operator<<(QDebug dbg, const qutim_sdk_0_3::Status &status)
{
	return operator <<(dbg, status.type());
}

QDataStream &operator<<(QDataStream &out, const qutim_sdk_0_3::Status &status)
{
	out << status.type() << status.text() << status.subtype() << status.icon().name();
	QHash<QString, QVariantHash>::const_iterator it = status.extendedInfos().constBegin();
	out << status.extendedInfos().count();
	for (; it != status.extendedInfos().constEnd(); it++)
		out << it.key() << it.value();
	return out;
}

QDataStream &operator>>(QDataStream &in, qutim_sdk_0_3::Status &status)
{
	int type;
	QString text;
	int subtype;
	QString icon;
	int count;
	in >> type >> text >> subtype >> icon >> count;
	status.setType(static_cast<qutim_sdk_0_3::Status::Type>(type));
	status.setText(text);
	status.setIcon(qutim_sdk_0_3::Icon(icon));

	for (int i = 0; i < count; i++) {
		QString key;
		QVariantHash hash;
		in >> key >> hash;
		status.setExtendedInfo(key, hash);
	}
	return in;
}


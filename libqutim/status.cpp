/****************************************************************************
 *  status.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "status.h"
#include "dynamicpropertydata_p.h"
#include "icon.h"
#include <QDebug>

namespace qutim_sdk_0_3
{
	class StatusPrivate : public DynamicPropertyData
	{
	public:
		StatusPrivate() : type(Status::Offline), subtype(0) {}
		StatusPrivate(const StatusPrivate &o) :
				DynamicPropertyData(o), text(o.text), name(o.name),
				icon(o.icon), type(o.type), subtype(o.subtype),
				extStatuses(o.extStatuses) {}
		~StatusPrivate() {}
		QString text;
		LocalizedString name;
		QIcon icon;
		Status::Type type;
		int subtype;
		QVariantHash extStatuses;

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
		QVariant getExtendedStatuses() const { return extStatuses; }
		void setExtendedStatuses(const QVariant &val) { extStatuses = val.toHash(); }

		void generateName();
	};

	typedef QVector<QSharedDataPointer<StatusPrivate> > StatusPrivateList;

	static void init_list(StatusPrivateList &list)
	{
		list.reserve(Status::Offline - Status::Connecting + 1);
		for (int i = Status::Connecting; i <= Status::Offline; i++) {
			Status::Type type = static_cast<Status::Type>(i);
			QSharedDataPointer<StatusPrivate> d(new StatusPrivate);
			d->type = type;
			d->generateName();
			d->icon = Status::createIcon(type);
			list.append(d);
		}
	}

	Q_GLOBAL_STATIC_WITH_INITIALIZER(StatusPrivateList, statusList, init_list(*x));

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
										 << "extendedStatuses";
		static QList<Getter> getters   = QList<Getter>()
										 << static_cast<Getter>(&StatusPrivate::getText)
										 << static_cast<Getter>(&StatusPrivate::getName)
										 << static_cast<Getter>(&StatusPrivate::getIcon)
										 << static_cast<Getter>(&StatusPrivate::getType)
										 << static_cast<Getter>(&StatusPrivate::getSubtype)
										 << static_cast<Getter>(&StatusPrivate::getExtendedStatuses);
		static QList<Setter> setters   = QList<Setter>()
										 << static_cast<Setter>(&StatusPrivate::setText)
										 << static_cast<Setter>(&StatusPrivate::setName)
										 << static_cast<Setter>(&StatusPrivate::setIcon)
										 << static_cast<Setter>(&StatusPrivate::setType)
										 << static_cast<Setter>(&StatusPrivate::setSubtype)
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


	typedef QHash<QByteArray, Status> StatusHash;
	Q_GLOBAL_STATIC(StatusHash, statusHash);

	Status Status::instance(Type type, const char *proto, int subtype)
	{
		QByteArray key;
		key += proto;
		key += '\0';
		key += QByteArray::number(int(type));
		key += '_';
		key += QByteArray::number(subtype);
		return statusHash()->value(key);
	}

	bool Status::remember(const Status &status, const char *proto)
	{
		QByteArray key;
		key += proto;
		key += '\0';
		key += QByteArray::number(int(status.type()));
		key += '_';
		key += QByteArray::number(status.subtype());
		if (statusHash()->contains(key))
			return false;
		statusHash()->insert(key, status);
		return true;
	}

	void Status::setExtendedInfo(const QString &name, const QVariantMap &status)
	{
		d->extStatuses.insert(name,status);
	}

	void Status::removeExtendedInfo(const QString &name)
	{
		d->extStatuses.remove(name);
	}

	QVariantMap Status::extendedInfo(const QString &name) const
	{
		return d->extStatuses.value(name).value<QVariantMap>();
	}

	QVariantHash Status::extendedInfos() const
	{
		return d->extStatuses;
	}

	void Status::setExtendedInfos(const QVariantHash &extInfos)
	{
		d->extStatuses = extInfos;
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

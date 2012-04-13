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

#include "kdesystemintegration.h"
#include <KUser>
#include <KTimeZone>
#include <KSystemTimeZone>
#include <QDebug>

KdeSystemIntegration::KdeSystemIntegration()
{
}

void KdeSystemIntegration::init()
{
}

bool KdeSystemIntegration::isAvailable() const
{
	QByteArray data = qgetenv("DESKTOP_SESSION");
	if (data.contains("kde")) {
		// Normal behaviour
		return true;
	} else if (data == "default") {
		// May be some old kde version
		data = qgetenv("KDE_SESSION_VERSION");
		bool ok = true;
		// We don't support KDE 3 or older
		if (data.toInt(&ok) >= 4 && ok)
			return true;
	}
	return false;
}

int KdeSystemIntegration::priority()
{
	return DesktopEnvironment;
}

inline QDateTime kde_get_date_time()
{
#if QT_VERSION_CHECK(4, 7, 0)
	return QDateTime::currentDateTimeUtc();
#else
	return QDateTime::currentDateTime().toUTC();
#endif
}

QVariant KdeSystemIntegration::doGetValue(Attribute attr, const QVariant &data) const
{
	Q_UNUSED(data);
	switch (attr) {
	case UserLogin:
		return KUser(KUser::UseRealUserID).loginName();
	case UserName:
		return KUser(KUser::UseRealUserID).property(KUser::FullName);
	case TimeZone:
		return KSystemTimeZones::local().offsetAtUtc(kde_get_date_time());
	case TimeZoneName:
		return KSystemTimeZones::local().name();
	case TimeZoneShortName:
		return KSystemTimeZones::local().abbreviation(kde_get_date_time());
	default:
		return QVariant();
	}
}

QVariant KdeSystemIntegration::doProcess(Operation act, const QVariant &data) const
{
	Q_UNUSED(data);
	switch (act) {
	default:
		return QVariant();
	}
}

bool KdeSystemIntegration::canHandle(Attribute attribute) const
{
	return attribute == UserLogin
	        || attribute == UserName
	        || attribute == TimeZone
	        || attribute == TimeZoneName
	        || attribute == TimeZoneShortName;
}

bool KdeSystemIntegration::canHandle(Operation operation) const
{
	Q_UNUSED(operation);
	return false;
}


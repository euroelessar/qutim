/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "jabber_global.h"

namespace Jabber
{
	static const QString full_stamp_str("yyyy-MM-ddThh:mm:ss.zzz");
	static const QString fullz_stamp_str("yyyy-MM-ddThh:mm:ss.zzzZ");
	static const QString date_time_stamp_str("yyyy-MM-ddThh:mm:ss");
	static const QString deprecated_stamp_str("yyyyMMddThh:mm:ss");
	static const QString time_stamp_str("hh:mm");

	QDateTime stamp2date(const std::string &std_stamp)
	{
		QDateTime date_time;
		QString stamp = QString::fromStdString(std_stamp);
		if (stamp.contains(QLatin1Char('-'))) {
			int num = stamp.indexOf(QLatin1Char('Z'));
			if (num<0)
				num = stamp.lastIndexOf(QLatin1Char('-'));

			QString time = stamp;
			time.truncate(num);

			if (num==19)
				date_time = QDateTime::fromString(time, date_time_stamp_str);
			else
				date_time = QDateTime::fromString(time, full_stamp_str);

			if (num>19) {
				QTime delta = QTime::fromString(stamp.right(5), time_stamp_str);
				int multi = 1;
				size_t sz = stamp.length() - 6;
				if (stamp.at(sz) == '+')
					multi = -1;
				date_time = date_time.addSecs(multi * (delta.minute() * 60 + delta.hour() * 3600));
			}
		}
		else // Deprecated format of stamp
			date_time = QDateTime::fromString(stamp, deprecated_stamp_str);

		if (!date_time.isValid())
			return QDateTime();
		date_time.setTimeSpec(Qt::UTC);
		return date_time.toLocalTime();
	}

	std::string date2stamp(const QDateTime &date_time)
	{
		return date_time.toUTC().toString(fullz_stamp_str).toStdString();
	}
}


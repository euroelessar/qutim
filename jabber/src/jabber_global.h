#ifndef JABBER_GLOBAL_H
#define JABBER_GLOBAL_H

#include <QDateTime>
#include <cstring>

namespace Jabber
{
	QDateTime stamp2date(const std::string &std_stamp);
	std::string date2stamp(const QDateTime &date_time);
}

#endif // JABBER_GLOBAL_H

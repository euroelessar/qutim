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
#ifndef MRIMSTATUS_H
#define MRIMSTATUS_H

#include <qutim/status.h>

using namespace qutim_sdk_0_3;

class MrimUserAgent;

class MrimStatus : public Status
{
public:
	MrimStatus(Type type = Offline);
	MrimStatus(const QString &uri,
			   const QString &title = QString(),
			   const QString &desc = QString());
	MrimStatus(const MrimStatus &status);
	MrimStatus(const Status &status);
	MrimStatus &operator =(const MrimStatus &o);
	MrimStatus &operator =(const Status &o);
	
	void setUserAgent(const MrimUserAgent &info);
	void setFlags(quint32 contactFlags);
	quint32 mrimType() const;
	
    QString toString() const;
    static Status fromString(const QString &uri,
                             const QString &title = QString(),
                             const QString &desc = QString());
    static LocalizedString defaultName(const Status &status);
};

#endif // MRIMSTATUS_H


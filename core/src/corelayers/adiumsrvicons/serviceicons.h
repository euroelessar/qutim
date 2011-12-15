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

#ifndef SERVICEICONS_H
#define SERVICEICONS_H

#include <qutim/iconloader.h>
#include <QtCore/QHash>

namespace Adium
{
	class ServiceIcons : public QObject, public qutim_sdk_0_3::IconWrapper
	{
		Q_OBJECT
		Q_INTERFACES(qutim_sdk_0_3::IconWrapper)
	public:
		ServiceIcons();
		virtual ~ServiceIcons();
		
		virtual QIcon doLoadIcon(const QString &name);
		virtual QMovie *doLoadMovie(const QString &name);
		virtual QString doIconPath(const QString &name, uint iconSize);
		virtual QString doMoviePath(const QString &name, uint iconSize);
	private:
		QHash<QString, QIcon> m_icons;
	};
}

#endif // SERVICEICONS_H


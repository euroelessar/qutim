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

#ifndef ICON_H
#define ICON_H

#include "libqutim_global.h"
#include <QIcon>

namespace qutim_sdk_0_3
{
	class LIBQUTIM_EXPORT Icon : public QIcon
	{
	public:
		// Qt 4.6 has ability for getting icons from system themes
		// but has no one for getting client-specific icons like
		// status icons and client icons (except installed clients)
		// so we create our own imlp of icon engine, which has wrapper
		// around QIcon::fromTheme
		enum Type { System, Status };

		// There is also another way: i.e. give names for icons
		// like "user-online-jabber", "user-online-icq" and so on,
		// names for clients should be like "miranda" or "qutim"
		Icon(const QString &name);

		Icon(const QIcon &icon);
		
		QString name() const;
	};
}

#endif // ICON_H


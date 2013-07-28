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

#include "icon.h"
#include "iconbackend_p.h"
#include "iconloader.h"
#include "objectgenerator.h"

namespace qutim_sdk_0_3
{
	Icon::Icon(const QString &name) : QIcon(IconLoader::loadIcon(name))
	{
	}

	Icon::Icon(const QIcon &icon) : QIcon(icon)
	{
	}

#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
	QString Icon::name() const
	{
		return QIcon::name();
	}
#else
# define IconNameHook 2
	struct IconPrivateHook
	{
		QIconEngine *engine;
		QAtomicInt ref;
		int serialNum;
		int detach_no;
		int engine_version;
	};
	
	QString Icon::name() const
	{
		QString name;
		DataPtr data = const_cast<Icon*>(this)->data_ptr();
		IconPrivateHook *p = reinterpret_cast<IconPrivateHook*>(data);
		if (!p || !p->engine || p->engine_version < 2)
			return name;
		p->engine->virtual_hook(IconNameHook, &name);
		return name;
	}
#endif
}


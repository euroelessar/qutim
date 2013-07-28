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

#ifndef SHORTCUT_P_H
#define SHORTCUT_P_H

#include "shortcut.h"
#include <QSet>

namespace qutim_sdk_0_3
{
class GeneralShortcutInfo;

typedef QHash<QString, GeneralShortcutInfo *> ShortcutInfoHash;
typedef void (*ShortcutHandler)(const QString &id, const QKeySequence &sequence);

struct ShortcutSelf
{
	ShortcutSelf();

	void updateSequence(const QString &id, const QKeySequence &secuence);
	static void addUpdateHandler(ShortcutHandler handler);
	static void removeUpdateHandler(ShortcutHandler handler);
	
	bool inited;
	ShortcutInfoHash hash;
	QList<ShortcutHandler> handlers;
};
}

#endif // SHORTCUT_P_H


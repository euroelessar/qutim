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
#ifndef BUDDY_P_H
#define BUDDY_P_H

#include "chatunit_p.h"
#include "buddy.h"

namespace Ureen
{

class BuddyPrivate : public ChatUnitPrivate
{
	Q_DECLARE_PUBLIC(Buddy)
public:
	BuddyPrivate(Buddy *b) : ChatUnitPrivate(b) {}
	void _q_status_changed(const Ureen::Status &now,const Ureen::Status &old);

};

}

#endif // BUDDY_P_H


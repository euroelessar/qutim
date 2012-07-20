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
#ifndef ABSTRACTCONTACTLIST_H
#define ABSTRACTCONTACTLIST_H
#include <qutim/menucontroller.h>
#include "simplecontactlist_global.h"

namespace Core {
namespace SimpleContactList {

class TreeView;

using namespace qutim_sdk_0_3;

class AbstractContactModel;
class SIMPLECONTACTLIST_EXPORT AbstractContactListWidget
{
public:
	virtual ~AbstractContactListWidget() {}
	virtual void addButton(ActionGenerator *generator) = 0;
	virtual void removeButton(ActionGenerator *generator) = 0;
	virtual TreeView *contactView() { Q_ASSERT(!"Implement me"); return 0; }
};

} // namespace SimpleContactList
} // namespace Core

Q_DECLARE_INTERFACE(Core::SimpleContactList::AbstractContactListWidget, "org.qutim.core.AbstractContactListWidget")

#endif // ABSTRACTCONTACTLIST_H


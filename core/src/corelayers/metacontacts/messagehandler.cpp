/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2012 Sergei Lopatin <magist3r@gmail.com>
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

#include "messagehandler.h"
#include <qutim/debug.h>
#include <qutim/metacontact.h>
#include <qutim/account.h>
#include <qutim/protocol.h>
#include "metacontactimpl.h"

using namespace qutim_sdk_0_3;

namespace Core
{
namespace MetaContacts
{
MetaContactMessageHandler::MetaContactMessageHandler()
{
}

MessageHandlerAsyncResult MetaContactMessageHandler::doHandle(Message &message)
{
	if (message.isIncoming() && !qobject_cast<MetaContactImpl*>(message.chatUnit())) {
		Q_ASSERT(message.chatUnit());
		if (MetaContactImpl *contact = qobject_cast<MetaContactImpl*>(message.chatUnit()->metaContact())) {
			Contact *rawContact = 0;
			ChatUnit *u = message.chatUnit();
			while (u) {
			   if ((rawContact = qobject_cast<Contact*>(u)))
						break;
					u = u->upperUnit();
			}
			if (rawContact && contact->getActiveContact() != rawContact)
				contact->setActiveContact(rawContact);
		}
	}

	return makeAsyncResult(Accept, QString());
}

}
}

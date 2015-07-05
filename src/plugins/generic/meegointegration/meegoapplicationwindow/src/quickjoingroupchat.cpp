/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "quickjoingroupchat.h"
#include <qutim/icon.h>
#include <qutim/menucontroller.h>
#include "joingroupchatwrapper.h"
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/servicemanager.h>
#include <qutim/groupchatmanager.h>
#include <QApplication>

namespace MeegoIntegration
{

static bool isSupportGroupchat()
{
	foreach (GroupChatManager *manager, GroupChatManager::allManagers()) {
		Status::Type status = manager->account()->status().type();
		if (status != Status::Offline && status != Status::Connecting)
			return true;
	}
	return false;
}

JoinGroupChatGenerator::JoinGroupChatGenerator(QObject* module)
	: ActionGenerator(Icon("meeting-attending"),
					  QT_TRANSLATE_NOOP("JoinGroupChat", "Join groupchat"),
					  module,
					  SLOT(onJoinGroupChatTriggered()))
{
}

void JoinGroupChatGenerator::showImpl(QAction *action, QObject *)
{
	action->setEnabled(isSupportGroupchat());
}

QuickJoinGroupChat::QuickJoinGroupChat()
{
	ServicePointer<MenuController> contactList("ContactList");
	if (contactList) {
		m_button.reset(new JoinGroupChatGenerator(this));
		contactList->addAction(m_button.data());
	}
}

QuickJoinGroupChat::~QuickJoinGroupChat()
{
	if (m_button) {
		ServicePointer<MenuController> contactList("ContactList");
		contactList->removeAction(m_button.data());
		m_button.reset(0);
	}
}

void QuickJoinGroupChat::onJoinGroupChatTriggered()
{
	JoinGroupChatWrapper::showDialog();
}

}


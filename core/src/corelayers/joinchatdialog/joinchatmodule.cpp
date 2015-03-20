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

#include "joinchatmodule.h"
#include <qutim/servicemanager.h>
#include <qutim/account.h>
#include <qutim/status.h>
#include <qutim/icon.h>
#include <qutim/systemintegration.h>
#include <QApplication>

using namespace qutim_sdk_0_3;

namespace Core
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

JoinChatModule::JoinChatModule() : m_action(new JoinChatGenerator(this))
{
	ServicePointer<MenuController> contactList("ContactList");
	if (contactList)
		contactList.data()->addAction(m_action.data());
}

JoinChatModule::~JoinChatModule()
{
	ServicePointer<MenuController> contactList("ContactList");
	if (contactList)
		contactList.data()->removeAction(m_action.data());
}

void JoinChatModule::onJoinGroupChatTriggered(const QString &url)
{
	if (!m_dialog)
		m_dialog = new JoinChatDialog(QApplication::activeWindow());
	if(!url.isEmpty())
		m_dialog.data()->setUri(url);
	SystemIntegration::show(m_dialog.data());
}

JoinChatGenerator::JoinChatGenerator(QObject *module)
    : ActionGenerator(Icon("meeting-attending"),
          QT_TRANSLATE_NOOP("JoinGroupChat", "Join groupchat"),
		  module, SLOT(onJoinGroupChatTriggered()))
{

}

void JoinChatGenerator::showImpl(QAction *action, QObject *)
{
	action->setEnabled(isSupportGroupchat());
}
}


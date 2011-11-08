/****************************************************************************
 *  joingroupchatmodule.cpp
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "joingroupchatmodule.h"
#include <qutim/icon.h>
#include <qutim/menucontroller.h>
#include "joingroupchat.h"
#include <qutim/account.h>
#include <qutim/protocol.h>
#include <qutim/servicemanager.h>
#include <qutim/groupchatmanager.h>
#include <QApplication>

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

JoinGroupChatModule::JoinGroupChatModule()
{
	QObject *contactList = ServiceManager::getByName("ContactList");
	if (contactList) {
		MenuController *controller = qobject_cast<MenuController*>(contactList);
		Q_ASSERT(controller);
        m_gen.reset(new JoinGroupChatGenerator(this));
        controller->addAction(m_gen.data());
	}
}

JoinGroupChatModule::~JoinGroupChatModule()
{

}

void JoinGroupChatModule::onJoinGroupChatTriggered()
{
	if (!m_chat)
		m_chat = new JoinGroupChat(qApp->activeWindow());
	m_chat.data()->setParent(QApplication::activeWindow());
#if defined (QUTIM_MOBILE_UI)
	m_chat.data()->showMaximized();
#else
	centerizeWidget(m_chat.data());
	m_chat.data()->show();
#endif
}

JoinGroupChatGenerator::JoinGroupChatGenerator(QObject* module):
	ActionGenerator(Icon("meeting-attending"),
		QT_TRANSLATE_NOOP("JoinGroupChat", "Join groupchat"),
		module,
		SLOT(onJoinGroupChatTriggered())
		)
{

}

void JoinGroupChatGenerator::showImpl(QAction *action, QObject *)
{
	action->setEnabled(isSupportGroupchat());
}
}

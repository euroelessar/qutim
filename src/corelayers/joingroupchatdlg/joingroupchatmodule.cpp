/****************************************************************************
 *  joingroupchatmodule.cpp
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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
#include <libqutim/icon.h>
#include <libqutim/menucontroller.h>
#include "joingroupchat.h"
#include <libqutim/account.h>
#include <libqutim/protocol.h>

namespace Core
{
	bool isSupportGroupchat()
	{
		foreach (Protocol *p,allProtocols()) {
			bool support = p->data(qutim_sdk_0_3::Protocol::ProtocolSupportGroupChat).toBool();
			if (support) {
				foreach (Account *a,p->accounts()) {
					if (a->status() != Status::Offline) {
						return true;
					}
				}
			}
		}
		return false;
	}
	
	JoinGroupChatModule::JoinGroupChatModule()
	{
		QObject *contactList = getService("ContactList");
		if (contactList) {
			MenuController *controller = qobject_cast<MenuController*>(contactList);
			Q_ASSERT(controller);
			static QScopedPointer<ActionGenerator> button(new JoinGroupChatGenerator(this));
			controller->addAction(button.data());
		}
	}

	JoinGroupChatModule::~JoinGroupChatModule()
	{

	}

	void JoinGroupChatModule::onJoinGroupChatTriggered()
	{
		if (!m_chat)
			m_chat = new JoinGroupChat();
		
#if defined (QUTIM_MOBILE_UI)
		m_chat->showMaximized();
#else
		m_chat->show();
		centerizeWidget(m_chat);
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

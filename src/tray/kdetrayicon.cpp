/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "kdetrayicon.h"
#include <qutim/extensioninfo.h>
#include <KStatusNotifierItem>
#include <KMenu>
#include <QDebug>
#include <QMetaType>
#include <qutim/icon.h>

using namespace qutim_sdk_0_3;

namespace KdeIntegration
{
	class ProtocolSeparatorActionGenerator : public ActionGenerator
	{
	public:
		ProtocolSeparatorActionGenerator(Protocol *proto, const ExtensionInfo &info) :
				ActionGenerator(info.icon(), metaInfo(info.generator()->metaObject(),"Protocol"), 0, 0)
		{
			setType(-1);
			m_proto = proto;
		}

		virtual QObject *generateHelper() const
		{
			if (m_proto->accounts().isEmpty())
				return NULL;
			QAction *action = prepareAction(new QAction(NULL));

			QFont font = action->font();
			font.setBold(true);
			action->setFont(font);
			return action;
		}
		
		virtual ~ProtocolSeparatorActionGenerator()
		{
		}
	private:
		Protocol *m_proto;
	};
	
	void StatusAction::onStatusChanged(Status status)
	{
		setIcon(status.icon());
	}
	
	StatusAction::StatusAction(QObject* parent): QAction(parent)
	{
	
	}
	
	class AccountMenuActionGenerator : public ActionGenerator
	{
	public:
		AccountMenuActionGenerator(Account *account) :
				ActionGenerator(QIcon(), account->id(), 0, 0)
		{
			m_account = account;
			setType(-1);
		}
	
		virtual QObject *generateHelper() const
		{
			StatusAction *action = new StatusAction(0);
			prepareAction(action);
			action->setIcon(m_account->status().icon());
			QMenu *menu = m_account->menu(false);
			QObject::connect(action, SIGNAL(destroyed()), menu, SLOT(deleteLater()));
			QObject::connect(m_account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
							 action, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
			action->setMenu(menu);
			return action;
		}
	private:
		Account *m_account;
	};
}

using namespace KdeIntegration;

KdeTrayIcon::KdeTrayIcon(QObject *parent) : MenuController(parent)
{
	QObject *contactList = getService("ContactList");
	setMenuOwner(qobject_cast<MenuController*>(contactList));
	QMenu *m = menu(false);
	KMenu *kmenu = qobject_cast<KMenu*>(m);
	if (!kmenu) {
		delete m;
		return;
	}
	kmenu->addTitle(Icon("qutim"), QLatin1String("qutIM"), kmenu->actions().value(0, 0));
	m_item = new KdeStatusNotifierItem(this);
	m_item->setStandardActionsEnabled(false);
	m_item->setStatus(KStatusNotifierItem::Active);
	
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	QMap<QString, Protocol*> protocols;
	foreach (Protocol *proto, allProtocols()) {
		protocols.insert(proto->id(), proto);
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
	}
	m_protocols = protocols.values();

	for (int i = 0; i < m_protocols.size(); i++) {
		ExtensionInfo info = m_protocols.at(i)->property("extensioninfo").value<ExtensionInfo>();
		ActionGenerator *gen = new ProtocolSeparatorActionGenerator(m_protocols.at(i), info);
		gen->setPriority(1 - i * 2);
		addAction(gen);
	}
	
	m_item->setCategory(KStatusNotifierItem::Communications);
//	if (contactList) {
//		QWidget *widget = contactList->property("widget").value<QWidget*>();
//		m_item->setAssociatedWidget(widget);
//	}
	m_item->setContextMenu(kmenu);
	QIcon icon;
	Icon qutimIcon("qutim");
	icon.addPixmap(qutimIcon.pixmap(16));
	icon.addPixmap(qutimIcon.pixmap(32));
	icon.addPixmap(qutimIcon.pixmap(64));
	icon.addPixmap(qutimIcon.pixmap(128));
	m_item->setIconByPixmap(icon);
	m_item->setAttentionIconByName("mail-unread-new");
}

void KdeTrayIcon::onActivated()
{
	if (m_sessions.isEmpty()) {
		if (QObject *obj = getService("ContactList"))
			obj->metaObject()->invokeMethod(obj, "changeVisibility");
	} else {
		m_sessions.first()->activate();
	}
}

void KdeTrayIcon::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),
			this, SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
	connect(session, SIGNAL(destroyed()), this, SLOT(onSessionDestroyed()));
}

void KdeTrayIcon::onSessionDestroyed()
{
	onUnreadChanged(MessageList());
}

void KdeTrayIcon::onUnreadChanged(qutim_sdk_0_3::MessageList unread)
{
	ChatSession *session = static_cast<ChatSession*>(sender());
	Q_ASSERT(session != NULL);
	MessageList::iterator itr = unread.begin();
	while (itr != unread.end()) {
		if (itr->property("silent", false))
			itr = unread.erase(itr);
		else
			++itr;
	}
	bool empty = m_sessions.isEmpty();
	if (unread.isEmpty()) {
		m_sessions.removeOne(session);
	} else if (!m_sessions.contains(session)) {
		m_sessions.append(session);
	} else {
		return;
	}
	if (empty == m_sessions.isEmpty())
		return;
	
	if (m_sessions.isEmpty())
		m_item->setStatus(KStatusNotifierItem::Active);
	else
		m_item->setStatus(KStatusNotifierItem::NeedsAttention);
}

void KdeTrayIcon::onAccountDestroyed(QObject *obj)
{
	ActionGenerator *gen = m_actions.take(static_cast<Account*>(obj));
	removeAction(gen);
	delete gen;
}

void KdeTrayIcon::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	if (m_actions.contains(account))
		return;
	m_accounts << account;
	ActionGenerator *gen = new AccountMenuActionGenerator(account);
	gen->setPriority(- m_protocols.indexOf(account->protocol()) * 2);
	m_actions.insert(account, gen);
	addAction(gen);
	connect(account, SIGNAL(destroyed(QObject*)), this, SLOT(onAccountDestroyed(QObject*)));
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
	if (!m_activeAccount) {
		if (account->status().type() != Status::Offline)
			m_activeAccount = account;
		m_currentIcon = account->status().icon();
		m_item->setOverlayIconByPixmap(m_currentIcon);
	}
}

void KdeTrayIcon::onStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Account *account = qobject_cast<Account*>(sender());
	if (account == m_activeAccount || !m_activeAccount) {
		m_activeAccount = account;
		if (account->status().type() == Status::Offline) {
			m_activeAccount = 0;
		}
		m_currentIcon = status.icon();
	}
	if (!m_activeAccount) {
		foreach (Account *acc, m_accounts) {
			if (acc->status().type() != Status::Offline) {
				m_activeAccount = acc;
				m_currentIcon = acc->status().icon();
				break;
			}
		}
	}
	m_item->setOverlayIconByPixmap(m_currentIcon);
}

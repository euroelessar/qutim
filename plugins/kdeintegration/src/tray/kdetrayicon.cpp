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
#include <qutim/metaobjectbuilder.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <qutim/utils.h>
#include <KStatusNotifierItem>
#include <KMenu>
#include <QDebug>
#include <QMetaType>
#include <QApplication>

using namespace qutim_sdk_0_3;

#define ICON_ONLINE QLatin1String("qutim-online")
#define ICON_OFFLINE QLatin1String("qutim-offline")
#define ICON_NEW_MESSAGE QLatin1String("mail-message-new-qutim")

namespace KdeIntegration
{
class ProtocolSeparatorActionGenerator : public ActionGenerator
{
public:
	ProtocolSeparatorActionGenerator(Protocol *proto, const ExtensionInfo &info) :
		ActionGenerator(info.icon(), MetaObjectBuilder::info(info.generator()->metaObject(),"Protocol"), 0, 0)
	{
		setType(-1);
		m_proto = proto;
	}

	virtual QObject *generateHelper() const
	{
		//			if (m_proto->accounts().isEmpty())
		//				return NULL;
		if (m_action)
			return m_action;

		m_action = prepareAction(new QAction(NULL));
		ensureVisibility();

		QFont font = m_action->font();
		font.setBold(true);
		m_action->setFont(font);
		return m_action;
	}

	virtual ~ProtocolSeparatorActionGenerator()
	{
	}

	void ensureVisibility() const
	{
		if (m_action)
			m_action->setVisible(!m_proto->accounts().isEmpty());
	}
	
	void showImpl(QAction *, QObject *)
	{
		ensureVisibility();
	}

private:
	Protocol *m_proto;
	mutable QPointer<QAction> m_action;
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
		QObject::connect(m_account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
						 action, SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
		QObject::connect(action, SIGNAL(destroyed()), menu, SLOT(deleteLater()));
		action->setMenu(menu);
		return action;
	}
private:
	Account *m_account;
};
}

using namespace KdeIntegration;

KdeTrayIcon::KdeTrayIcon(QObject *parent) :
	MenuController(parent),
	NotificationBackend("Tray")
{
	QObject *contactList = ServiceManager::getByName("ContactList");
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
	
	QMap<QString, Protocol*> protocols;
	foreach (Protocol *proto, Protocol::all()) {
		protocols.insert(proto->id(), proto);
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)),
				this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
	}
	m_protocols = protocols.values();

	for (int i = 0; i < m_protocols.size(); i++) {
		ExtensionInfo info = m_protocols.at(i)->property("extensioninfo").value<ExtensionInfo>();
		ProtocolSeparatorActionGenerator *gen =
				new ProtocolSeparatorActionGenerator(m_protocols.at(i), info);
		m_protocolActions.append(gen);
		gen->setPriority(1 - i * 2);
		addAction(gen);
		foreach (Account *account, m_protocols.at(i)->accounts())
			onAccountCreated(account);
	}
	
	m_item->setCategory(KStatusNotifierItem::Communications);
	//	if (contactList) {
	//		QWidget *widget = contactList->property("widget").value<QWidget*>();
	//		m_item->setAssociatedWidget(widget);
	//	}
	m_item->setContextMenu(kmenu);
	m_item->setIconByName(ICON_OFFLINE);
	m_item->setAttentionIconByName(ICON_NEW_MESSAGE);
	qApp->setQuitOnLastWindowClosed(false);
}

void KdeTrayIcon::onActivated()
{
	if (m_notifications.isEmpty()) {
		if (QObject *obj = ServiceManager::getByName("ContactList"))
			obj->metaObject()->invokeMethod(obj, "changeVisibility");
	} else {
		m_notifications.first()->accept();
	}
}

void KdeTrayIcon::onNotificationAcceptedOrCanceled()
{
	Notification *notif = sender_cast<Notification*>(sender());
	m_notifications.removeOne(notif);
	deref(notif);

	if (m_notifications.isEmpty())
		m_item->setStatus(KStatusNotifierItem::Active);
}

void KdeTrayIcon::onAccountDestroyed(QObject *obj)
{
	ActionGenerator *gen = m_actions.take(static_cast<Account*>(obj));
	removeAction(gen);
	delete gen;
	validateProtocolActions();
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
		m_item->setOverlayIconByPixmap(convertToPixmaps(m_currentIcon));
	}
	validateProtocolActions();
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
	QString iconName = ICON_OFFLINE;
	if (!m_activeAccount) {
		foreach (Account *acc, m_accounts) {
			if (acc->status().type() != Status::Offline) {
				m_activeAccount = acc;
				m_currentIcon = acc->status().icon();
				iconName = ICON_ONLINE;
				break;
			}
		}
	}
	m_item->setIconByName(iconName);
	m_item->setOverlayIconByPixmap(convertToPixmaps(m_currentIcon));
}

void KdeTrayIcon::handleNotification(Notification *notification)
{
	// Skip the notification if we already have a notification with the same object.
	// We don't want users to frantically click the tray a dozen times just to make it
	// not blink, right?
	QObject *obj = notification->request().object();
	foreach (Notification *notif, m_notifications) {
		if (obj == notif->request().object())
			return;
	}

	ref(notification);
	m_notifications << notification;

	m_item->setStatus(KStatusNotifierItem::NeedsAttention);

	connect(notification, SIGNAL(accepted()),
			SLOT(onNotificationAcceptedOrCanceled()));
	connect(notification, SIGNAL(ignored()),
			SLOT(onNotificationAcceptedOrCanceled()));
}

QIcon KdeTrayIcon::convertToPixmaps(const QIcon &source)
{
	QIcon icon;
	icon.addPixmap(source.pixmap(16));
	icon.addPixmap(source.pixmap(32));
	//	icon.addPixmap(source.pixmap(64));
	//	icon.addPixmap(source.pixmap(128));
	return source;
}

void KdeTrayIcon::validateProtocolActions()
{
	foreach (KdeIntegration::ProtocolSeparatorActionGenerator *gen, m_protocolActions)
		gen->ensureVisibility();
}

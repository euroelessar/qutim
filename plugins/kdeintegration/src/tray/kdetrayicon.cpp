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

#include "kdetrayicon.h"
#include <qutim/extensioninfo.h>
#include <qutim/metaobjectbuilder.h>
#include <qutim/icon.h>
#include <qutim/servicemanager.h>
#include <qutim/utils.h>
#include <KStatusNotifierItem>
#include <KIconLoader>
#include <KMenu>
#include <QDebug>
#include <QMetaType>
#include <QApplication>

using namespace qutim_sdk_0_3;

#define ICON_ONLINE QLatin1String("qutim-online")
#define ICON_OFFLINE QLatin1String("qutim-offline")
#define ICON_NEW_MESSAGE QLatin1String("qutim-message-new")

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
			return m_action.data();

		m_action = prepareAction(new QAction(NULL));
		ensureVisibility();

		QFont font = m_action.data()->font();
		font.setBold(true);
		m_action.data()->setFont(font);
		return m_action.data();
	}

	virtual ~ProtocolSeparatorActionGenerator()
	{
	}

	void ensureVisibility() const
	{
		if (m_action)
			m_action.data()->setVisible(!m_proto->accounts().isEmpty());
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
	NotificationBackend("Tray"),
    m_currentIcon(QIcon())
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
	debug() << KIconLoader::global()->iconPath(ICON_ONLINE, KIconLoader::Panel)
	        << KIconLoader::global()->iconPath(ICON_OFFLINE, KIconLoader::Panel)
	        << KIconLoader::global()->iconPath(ICON_NEW_MESSAGE, KIconLoader::Panel);
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

void KdeTrayIcon::onNotificationFinished()
{
	Notification *notif = sender_cast<Notification*>(sender());
	m_notifications.removeOne(notif);

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
	debug() << "Account added" << account->id();
	m_accounts << account;
	ActionGenerator *gen = new AccountMenuActionGenerator(account);
	gen->setPriority(- m_protocols.indexOf(account->protocol()) * 2);
	m_actions.insert(account, gen);
	addAction(gen);
	connect(account, SIGNAL(destroyed(QObject*)),
	        SLOT(onAccountDestroyed(QObject*)));
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(validateIcon()));
	validateIcon();
	validateProtocolActions();
}

void KdeTrayIcon::handleNotification(Notification *notification)
{
	ref(notification);
	m_notifications << notification;

	m_item->setStatus(KStatusNotifierItem::NeedsAttention);
	connect(notification, SIGNAL(finished(qutim_sdk_0_3::Notification::State)),
			SLOT(onNotificationFinished()));
}

void KdeTrayIcon::validateIcon()
{
	QString iconName = ICON_OFFLINE;
	QString overlayIconName;
	Status::Type bestStatus = Status::Offline;
	foreach (Account *acc, m_accounts) {
		const Status status = acc->status();
		if (status != Status::Connecting && status.type() < bestStatus) {
			overlayIconName = Icon(status.icon()).name();
			iconName = ICON_ONLINE;
			bestStatus = status.type();
			break;
		}
	}
	m_item->setIconByName(iconName);
	m_item->setOverlayIconByName(overlayIconName);
}

void KdeTrayIcon::validateProtocolActions()
{
	foreach (KdeIntegration::ProtocolSeparatorActionGenerator *gen, m_protocolActions)
		gen->ensureVisibility();
}


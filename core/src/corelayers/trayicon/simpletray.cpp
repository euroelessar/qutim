/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#include "simpletray.h"
#include <qutim/extensioninfo.h>
#include "qutim/metaobjectbuilder.h"
#include <qutim/servicemanager.h>
#include <qutim/debug.h>
#include <qutim/settingslayer.h>
#include <qutim/utils.h>
#include <QApplication>
#include <QWidgetAction>
#include <QToolButton>
#include <QDateTime>
#include <QPainter>
#include <QPixmap>
#include <QString>

namespace Core
{

class ProtocolSeparatorActionGenerator : public ActionGenerator
{
public:
	ProtocolSeparatorActionGenerator(Protocol *proto, const ExtensionInfo &info);
	virtual QObject *generateHelper() const;
	virtual ~ProtocolSeparatorActionGenerator();
	void ensureVisibility() const;
	void showImpl(QAction *, QObject *);
private:
	Protocol *m_proto;
	QToolButton *m_btn;
	mutable QWeakPointer<QAction> m_action;
};

#ifdef Q_WS_WIN
class ClEventFilter : public QObject
{
	SimpleTray *tray_;
	QWidget *widget_;

public:
	ClEventFilter(SimpleTray *t, QWidget *w)
		: QObject(t)
	{
		tray_   = t;
		widget_ = w;
	}

	bool eventFilter(QObject *obj, QEvent *ev)
	{
		if (QEvent::ActivationChange == ev->type() && obj == widget_) {
			if (!widget_->isActiveWindow())
				tray_->clActivationStateChanged(widget_->isActiveWindow());
		}
		return false;
	}
};
#endif

SimpleTray::SimpleTray() :
	NotificationBackend("Tray")
{
	setDescription(QT_TR_NOOP("Blink icon in the tray"));

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		debug() << "No System Tray Available. Tray icon not loaded.";
		return;
	}
	m_activeAccount = 0;
	m_showGeneratedIcon = false;
	m_icon = new QSystemTrayIcon(this);
	m_icon->setIcon(m_currentIcon = Icon(QLatin1String("qutim-offline")));
	m_icon->show();
	m_mailIcon                = Icon(QLatin1String("qutim-message-new"));
	m_typingIcon              = Icon(QLatin1String("im-status-message-edit"));
	m_chatUserJoinedIcon      = Icon(QLatin1String("list-add-user-conference"));
	m_chatUserLeftIcon        = Icon(QLatin1String("list-remove-user-conference"));
	m_qutimIcon               = Icon(QLatin1String("qutim"));
	m_transferCompletedIcon   = Icon(QLatin1String("document-save-filetransfer-comleted"));
	m_birthdayIcon            = Icon(QLatin1String("view-calendar-birthday"));
	m_defaultNotificationIcon = Icon(QLatin1String("dialog-information"));
	connect(m_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(onActivated(QSystemTrayIcon::ActivationReason)));
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
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

	setMenuOwner(qobject_cast<MenuController*>(ServiceManager::getByName("ContactList")));
	QMenu *contextMenu = menu(false);
	connect(m_icon, SIGNAL(destroyed()), contextMenu, SLOT(deleteLater()));
	m_icon->setContextMenu(contextMenu);
	qApp->setQuitOnLastWindowClosed(false);

#ifdef Q_WS_WIN
	QWidget *clWindow = ServiceManager::getByName("ContactList")->property("widget").value<QWidget*>();
	clWindow->installEventFilter(new ClEventFilter(this, clWindow));
	activationStateChangedTime = QDateTime::currentMSecsSinceEpoch();
#endif

	m_settingsItem = new GeneralSettingsItem<SimpletraySettings>(
				Settings::Plugin, Icon("user-desktop"),
				QT_TRANSLATE_NOOP("Plugin", "Notification Area Icon"));
	Settings::registerItem(m_settingsItem);
	m_settingsItem->connect(SIGNAL(saved()), this, SLOT(reloadSettings()));
	reloadSettings();
}

SimpleTray::~SimpleTray()
{
	Settings::removeItem(m_settingsItem);
	delete m_settingsItem;
}

void SimpleTray::clActivationStateChanged(bool activated)
{
	activationStateChangedTime = activated ? 0 : QDateTime::currentMSecsSinceEpoch();
}

void SimpleTray::onActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger) {
		Notification *notif = currentNotification();
		if (!notif) {
			if (QObject *obj = ServiceManager::getByName("ContactList")) {
#ifdef Q_WS_WIN
				if (QDateTime::currentMSecsSinceEpoch() - activationStateChangedTime < 200) { // tested - enough
					obj->metaObject()->invokeMethod(obj, "changeVisibility");
					clActivationStateChanged(true);
				}
#endif
				obj->metaObject()->invokeMethod(obj, "changeVisibility");
			}
		} else {
			notif->accept();
		}
	}
}

void SimpleTray::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),
			this, SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
	connect(session, SIGNAL(destroyed()), this, SLOT(onSessionDestroyed()));
}

void SimpleTray::onSessionDestroyed()
{
	ChatSession *session = static_cast<ChatSession*>(sender());
	m_sessions.remove(session);
	updateGeneratedIcon();
}

void SimpleTray::onUnreadChanged(qutim_sdk_0_3::MessageList unread)
{
	ChatSession *session = sender_cast<ChatSession*>(sender());

	MessageList::iterator itr = unread.begin();
	while (itr != unread.end()) {
		if (itr->property("silent", false))
			itr = unread.erase(itr);
		else
			++itr;
	}

	if (unread.isEmpty())
		m_sessions.remove(session);
	else
		m_sessions.insert(session, unread.count());

	updateGeneratedIcon();
}

void SimpleTray::onNotificationFinished()
{
	Notification *notif = sender_cast<Notification*>(sender());
	deref(notif);
	Notification *current = currentNotification();
	if (!m_messageNotifications.removeOne(notif))
		if (!m_notifications.removeOne(notif))
			m_typingNotifications.removeOne(notif);
	if (current == notif)
		updateGeneratedIcon();
}

void SimpleTray::reloadSettings()
{
	Config cfg(traySettingsFilename);
	m_showNumber = cfg.value("showNumber", SimpletraySettings::CounterDontShow);
	m_blink = cfg.value("blink", true);
	m_showIcon = cfg.value("showIcon", true);
}

void SimpleTray::handleNotification(Notification *notification)
{
	Notification::Type type = notification->request().type();

	if (type == Notification::IncomingMessage ||
		type == Notification::OutgoingMessage ||
		type == Notification::ChatIncomingMessage ||
		type == Notification::ChatOutgoingMessage)
	{
		m_messageNotifications << notification;
	} else if (type == Notification::UserTyping) {
		m_typingNotifications << notification;
	} else {
		m_notifications << notification;
	}

	ref(notification);
	connect(notification, SIGNAL(finished(qutim_sdk_0_3::Notification::State)),
			SLOT(onNotificationFinished()));

	if (!m_iconTimer.isActive() && m_blink && m_showIcon) {
		m_iconTimer.start(500, this);
		m_showGeneratedIcon = true;
	}
	if (notification == currentNotification())
		updateGeneratedIcon();
}

void SimpleTray::timerEvent(QTimerEvent *timer)
{
	if (timer->timerId() != m_iconTimer.timerId()) {
		QObject::timerEvent(timer);
	} else {
		m_icon->setIcon(m_showGeneratedIcon ? m_generatedIcon : m_currentIcon);
		m_showGeneratedIcon = !m_showGeneratedIcon;
	}
}

QIcon SimpleTray::unreadIcon()
{
	int number = 0;

	switch (m_showNumber) {
	default:
	case SimpletraySettings::CounterDontShow:
		return m_mailIcon;
	case SimpletraySettings::CounterShowMessages:
		foreach (quint64 c, m_sessions)
			number += c;
		break;
	case SimpletraySettings::CounterShowSessions:
		number = m_sessions.count();
		break;
	}

	QIcon icon;
	generateIconSizes(m_mailIcon, icon, number);
	return icon;
}

QIcon SimpleTray::getIconForNotification(Notification *notification)
{
	switch (notification->request().type()) {
	case Notification::IncomingMessage:
	case Notification::OutgoingMessage:
	case Notification::ChatIncomingMessage:
	case Notification::ChatOutgoingMessage:
		return unreadIcon();
	case Notification::UserTyping:
		return m_typingIcon;
	case Notification::ChatUserJoined:
		return m_chatUserJoinedIcon;
	case Notification::ChatUserLeft:
		return m_chatUserLeftIcon;
	case Notification::AppStartup:
		return m_qutimIcon;
	case Notification::FileTransferCompleted:
		return m_transferCompletedIcon;
	case Notification::UserHasBirthday:
		return m_birthdayIcon;
	case Notification::UserOnline:
	case Notification::UserOffline:
	case Notification::UserChangedStatus:
	case Notification::BlockedMessage:
	case Notification::System:
		return m_defaultNotificationIcon;
	}
	return QIcon();
}

static QIcon addIcon(const QIcon &backing, QIcon &icon, const QSize &size, int number)
{
	QFont f = QApplication::font();
	QPixmap px(backing.pixmap(size));
	QPainter p(&px);
	f.setPixelSize(px.height()/1.5);
	p.setFont(f);
	p.drawText(px.rect(), Qt::AlignHCenter | Qt::AlignVCenter, QString::number(number));
	icon.addPixmap(px);
	return icon;
}

void SimpleTray::generateIconSizes(const QIcon &backing, QIcon &icon, int number)
{
	foreach (QSize sz, backing.availableSizes()) {
		addIcon(backing, icon, sz, number);
	}
	//for SVG icons
	QRect geometry = m_icon->geometry();
	QSize size = geometry.size();
	addIcon(backing, icon, size, number);
}

void SimpleTray::updateGeneratedIcon()
{
	Notification *notif = currentNotification();
	if (!notif) {
		if (m_iconTimer.isActive())
			m_iconTimer.stop();
		m_icon->setIcon(m_currentIcon);
		m_showGeneratedIcon = false;
	} else if (m_showIcon) {
		m_generatedIcon = getIconForNotification(notif);
		if (!m_blink || m_showGeneratedIcon) {
			m_icon->setIcon(m_generatedIcon);
			m_showGeneratedIcon = true;
		}
	}
}

Notification *SimpleTray::currentNotification()
{
	// Message notifications have highest priority
	if (!m_messageNotifications.isEmpty())
		return m_messageNotifications.first();

	else if (!m_notifications.isEmpty())
		return m_notifications.first();

	// Typing notifications have lowest priority
	else if (!m_typingNotifications.isEmpty())
		return m_typingNotifications.first();

	return 0;
}


//---------------------------------------------------------------------------
// Accounts context menu implementation
//---------------------------------------------------------------------------

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

static QIcon iconForStatus(const Status &status)
{
	//TODO create icon
	if (status.type() == Status::Offline)
		return Icon("qutim-offline");
	else if (status.type() == Status::Connecting)
		return Icon("qutim-offline");
	else
		return Icon("qutim-online");
}

ProtocolSeparatorActionGenerator::ProtocolSeparatorActionGenerator(Protocol *proto, const ExtensionInfo &info) :
	ActionGenerator(info.icon(), MetaObjectBuilder::info(info.generator()->metaObject(),"Protocol"), 0, 0)
{
	setType(-1);
	m_proto = proto;
}

QObject *ProtocolSeparatorActionGenerator::generateHelper() const
{
	if (m_action)
		return m_action.data();
	QAction *action = prepareAction(new QAction(NULL));
	QFont font = action->font();
	font.setBold(true);
	action->setFont(font);
#ifndef Q_WS_MAC
	QToolButton *m_btn = new QToolButton();
	QWidgetAction *widget = new QWidgetAction(action);
	m_action = QWeakPointer<QAction>(widget);
	widget->setDefaultWidget(m_btn);
	QObject::connect(widget, SIGNAL(destroyed()), action, SLOT(deleteLater()));
	QObject::connect(widget, SIGNAL(destroyed()), m_btn, SLOT(deleteLater()));
	m_btn->setDefaultAction(action);
	m_btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	m_btn->setDown(true); // prevent hover style changes in some styles
	return widget;
#else
	return action;
#endif
}

ProtocolSeparatorActionGenerator::~ProtocolSeparatorActionGenerator()
{
}

void ProtocolSeparatorActionGenerator::ensureVisibility() const
{
	if (m_action)
		m_action.data()->setVisible(!m_proto->accounts().isEmpty());
}

void ProtocolSeparatorActionGenerator::showImpl(QAction *, QObject *)
{
	ensureVisibility();
}

StatusAction::StatusAction(QObject* parent): QAction(parent)
{

}

void StatusAction::onStatusChanged(Status status)
{
	setIcon(status.icon());
}

void SimpleTray::onAccountDestroyed(QObject *obj)
{
	ActionGenerator *gen = m_actions.take(static_cast<Account*>(obj));
	removeAction(gen);
	delete gen;
	validateProtocolActions();
}

void SimpleTray::onAccountCreated(qutim_sdk_0_3::Account *account)
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
		m_currentIcon = iconForStatus(account->status());
		if (!m_showGeneratedIcon)
			m_icon->setIcon(m_currentIcon);
	}
	validateProtocolActions();
}

void SimpleTray::onStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Account *account = qobject_cast<Account*>(sender());
	if (account == m_activeAccount || !m_activeAccount) {
		m_activeAccount = account;
		if (account->status().type() == Status::Offline) {
			m_activeAccount = 0;
		}
		m_currentIcon =iconForStatus(status);
	}
	if (!m_activeAccount) {
		foreach (Account *acc, m_accounts) {
			if (acc->status().type() != Status::Offline) {
				m_activeAccount = acc;
				m_currentIcon = iconForStatus(acc->status());
				break;
			}
		}
	}
	if (!m_showGeneratedIcon)
		m_icon->setIcon(m_currentIcon);
}

void SimpleTray::validateProtocolActions()
{
	foreach (ProtocolSeparatorActionGenerator *gen, m_protocolActions)
		gen->ensureVisibility();
}

}


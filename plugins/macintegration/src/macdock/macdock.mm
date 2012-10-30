/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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


#include "macdock.h"
#include <qutim/icon.h>
#include <qutim/extensioninfo.h>
#include <qutim/servicemanager.h>
#include <qutim/config.h>
#include <QApplication>
#include <QLabel>
#import <AppKit/NSDockTile.h>

extern void qt_mac_set_dock_menu(QMenu *);

@interface DockIconHandler : NSObject
{
	@public
	MacIntegration::MacDock *macDock;
}
- (void)handleDockClickEvent:(NSAppleEventDescriptor*)event withReplyEvent:(NSAppleEventDescriptor*)replyEvent;
@end

@implementation DockIconHandler
- (void)handleDockClickEvent:(NSAppleEventDescriptor*)event withReplyEvent:(NSAppleEventDescriptor*)replyEvent
{
	if (macDock)
		macDock->dockIconClickEvent();
}
@end

namespace MacIntegration
{
class MacDockPrivate
{
public:
	QMenu *dockMenu;
	QMenu *trayMenu;
	QHash<QMenu *, QActionGroup *> statusGroups;
	QMenu *chatMenu;
	QHash<ChatSession *, int> unreadSessions;
	QHash<ChatSession *, QAction *> aliveSessions;
	bool isTrayAvailable;
	bool isMail;
	QSystemTrayIcon *tray;
	QIcon standartIcon;
	QIcon offlineIcon;
	QIcon standartTrayIcon;
	QIcon offlineTrayIcon;
	QIcon *currentTrayIcon;
	QIcon mailIcon;
	DockIconHandler *dockHandler;
	QBasicTimer *timer;
};

MacDock::MacDock() : d_ptr(new MacDockPrivate())
{
	Q_D(MacDock);
	d->standartIcon = Icon("qutim-online");
	d->offlineIcon = Icon("qutim-offline");
	d->standartTrayIcon = Icon("qutim-online-mono");
	d->offlineTrayIcon = Icon("qutim-offline-mono");
	d->mailIcon = Icon("qutim-message-new-mono");
	createDockDeps();
	d->tray = 0;
	loadSettings();
	setStatusIcon();
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	foreach (qutim_sdk_0_3::Protocol *proto, qutim_sdk_0_3::Protocol::all()) {
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account *)),
				this, SLOT(onAccountCreated(qutim_sdk_0_3::Account *)));
		connect(proto, SIGNAL(accountRemoved(qutim_sdk_0_3::Account *)), this, SLOT(setStatusIcon()));
	}
	qApp->setQuitOnLastWindowClosed(false);
}

MacDock::~MacDock()
{
}

void MacDock::createDockDeps()
{
	Q_D(MacDock);
	d->dockMenu = new QMenu();
	d->statusGroups.insert(d->dockMenu, new QActionGroup(this));
	createStatuses(d->dockMenu);
	d->dockMenu->addSeparator();
	d->chatMenu = new QMenu(tr("Opened chats"), d->dockMenu);
	d->dockMenu->addMenu(d->chatMenu);
	d->chatMenu->setEnabled(false);
	qt_mac_set_dock_menu(d->dockMenu);
	d->dockHandler = [[DockIconHandler alloc] init];
	d->dockHandler->macDock = this;
	[[NSAppleEventManager sharedAppleEventManager]
			setEventHandler:d->dockHandler
		andSelector:@selector(handleDockClickEvent:withReplyEvent:)
	  forEventClass:kCoreEventClass
	  andEventID:kAEReopenApplication];
}

void MacDock::createTrayDeps()
{
	Q_D(MacDock);
	if (!QSystemTrayIcon::isSystemTrayAvailable() || !d->isTrayAvailable) {
		d->isTrayAvailable = false;
		return;
	}
	d->tray = new QSystemTrayIcon();
	d->timer = new QBasicTimer();
	connect(d->tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(onTrayActivated(QSystemTrayIcon::ActivationReason)));
	d->isTrayAvailable = true;
	d->trayMenu = new QMenu();
	d->statusGroups.insert(d->trayMenu, new QActionGroup(this));
	d->tray->setContextMenu(d->trayMenu);
	createStatuses(d->trayMenu);
	d->isMail = false;
	d->tray->show();
}

void MacDock::removeTrayDeps()
{
	Q_D(MacDock);
	if (!QSystemTrayIcon::isSystemTrayAvailable())
		return;
	d->tray->hide();
	d->tray->deleteLater();
	d->timer->stop();
	delete d->timer;
	d->statusGroups.take(d->trayMenu)->deleteLater();
	d->trayMenu->deleteLater();
}

void MacDock::createStatuses(QMenu *parent)
{
	createStatusAction(Status::Online, parent);
	createStatusAction(Status::FreeChat, parent);
	createStatusAction(Status::Away, parent);
	createStatusAction(Status::DND, parent);
	createStatusAction(Status::NA, parent);
	createStatusAction(Status::Invisible, parent);
	createStatusAction(Status::Offline, parent);
}

void MacDock::createStatusAction(Status::Type type, QMenu *parent)
{
	Status s = Status(type);
	QAction *act = new QAction(s.icon(), s.name(), parent);
	act->setCheckable(true);
	connect(act, SIGNAL(triggered(bool)), SLOT(onStatusChanged()));
	act->setParent(parent);
	act->setData(type);
	parent->addAction(act);
	d_func()->statusGroups.value(parent)->addAction(act);
}

void MacDock::onTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger)
		dockIconClickEvent();
}

void MacDock::setBadgeLabel(const QString &message)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString* mac_message = [[NSString alloc] initWithUTF8String: message.toUtf8().constData()];
	NSDockTile *dockTile = [NSApp dockTile];
	[dockTile setBadgeLabel: mac_message];
	[dockTile display];
	[pool release];
}

void MacDock::onStatusChanged()
{
	if (QAction *a = qobject_cast<QAction *>(sender())) {
		Status::Type type = static_cast<Status::Type>(a->data().value<int>());
		foreach(qutim_sdk_0_3::Protocol *proto, qutim_sdk_0_3::Protocol::all()) {
			foreach(Account *account, proto->accounts()) {
				Status status = account->status();
				status.setType(type);
				status.setSubtype(0);
				status.setProperty("changeReason",Status::ByUser);
				account->setStatus(status);
			}
		}
	}
}

void MacDock::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	Q_D(MacDock);
	d->chatMenu->setEnabled(true);
	QAction *action = new QAction(session->getUnit()->title(), d->chatMenu);
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), session, SLOT(activate()));
	d->chatMenu->addAction(action);
	d->aliveSessions.insert(session, action);
	connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
	connect(session, SIGNAL(activated(bool)), this, SLOT(onActivatedSession(bool)));
	connect(session, SIGNAL(destroyed()), SLOT(onSessionDestroyed()));
}

void MacDock::onSessionDestroyed()
{
	Q_D(MacDock);
	ChatSession *session = static_cast<ChatSession*>(sender());
	delete d->aliveSessions.take(session);
	onUnreadChanged(MessageList());
	if (d->aliveSessions.isEmpty())
		d->chatMenu->setEnabled(false);
}

void MacDock::onActivatedSession(bool state)
{
	ChatSession *session = static_cast<ChatSession*>(sender());
	d_func()->aliveSessions.value(session)->setChecked(state);
}

void MacDock::onUnreadChanged(const qutim_sdk_0_3::MessageList &unread)
{
	ChatSession *session = static_cast<ChatSession*>(sender());
	Q_ASSERT(session != NULL);
	Q_D(MacDock);
	if (unread.isEmpty()) {
		d->unreadSessions.remove(session);
		if (d->isTrayAvailable) {
			if (d->timer->isActive())
				d->timer->stop();
			d->tray->setIcon(*d->currentTrayIcon);
			d->isMail = false;
		}
	} else {
		d->unreadSessions.insert(session, unread.count());
		if (d->isTrayAvailable) {
			if (!d->timer->isActive())
				d->timer->start(500, this);
			d->tray->setIcon(d->mailIcon);
			d->isMail = true;
		}
	}
	int uCount = 0;
	foreach (ChatSession *s, d->aliveSessions.keys())
		if (d->unreadSessions.keys().contains(s)) {
			d->aliveSessions.value(s)->setText("✉ " + d->aliveSessions.value(s)->text().remove("✉ "));
			uCount += d->unreadSessions.value(s, 0);
		} else {
			d->aliveSessions.value(s)->setText(d->aliveSessions.value(s)->text().remove("✉ "));
		}
	setBadgeLabel(!uCount ? QString() : QString::number(uCount));
}

void MacDock::timerEvent(QTimerEvent *timer)
{
	Q_D(MacDock);
	if (timer->timerId() != d->timer->timerId()) {
		QObject::timerEvent(timer);
	} else {
		d->tray->setIcon(d->isMail ? *d->currentTrayIcon : d->mailIcon);
		d->isMail = !d->isMail;
	}
}

void MacDock::dockIconClickEvent()
{
	Q_D(MacDock);
	if (QObject *obj = ServiceManager::getByName("ContactList"))
		QMetaObject::invokeMethod(obj, "show");
	if (!d->unreadSessions.isEmpty())
		d->unreadSessions.keys().first()->activate();
}

void MacDock::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(setStatusIcon()));
}

void MacDock::setStatusIcon()
{
	Q_D(MacDock);
	bool isOnline = false;
	bool start = true;
	Status::Type globalStatus = Status::Offline;
	bool isStatusGlobal = true;
	foreach(qutim_sdk_0_3::Protocol *protocol, qutim_sdk_0_3::Protocol::all()) {
		foreach(qutim_sdk_0_3::Account *account, protocol->accounts()) {
			Status::Type type = account->status().type();
			switch (type) {
			case Status::Online:
			case Status::FreeChat:
			case Status::Away:
			case Status::NA:
			case Status::DND:
			case Status::Invisible:
				isOnline = true;
				break;
			default:
				break;
			}
			if (start)
				globalStatus = type;
			start = false;
			if (type != globalStatus)
				isStatusGlobal = false;
		}
	}
	if (isOnline) {
		qApp->setWindowIcon(d->standartIcon);
		if (d->isTrayAvailable) {
			d->tray->setIcon(d->standartTrayIcon);
			d->currentTrayIcon = &d->standartTrayIcon;
		}
	} else {
		qApp->setWindowIcon(d->offlineIcon);
		if (d->isTrayAvailable) {
			d->tray->setIcon(d->offlineTrayIcon);
			d->currentTrayIcon = &d->offlineTrayIcon;
		}
	}
	if (isStatusGlobal) {
		foreach(QAction *action, d->statusGroups.value(d->dockMenu)->actions()) {
			Status::Type type = static_cast<Status::Type>(action->data().value<int>());
			if (type == globalStatus)
				action->setChecked(true);
		}
		if (d->isTrayAvailable)
			foreach(QAction *action, d->statusGroups.value(d->trayMenu)->actions()) {
				Status::Type type = static_cast<Status::Type>(action->data().value<int>());
				if (type == globalStatus)
					action->setChecked(true);
			}
	} else {
		foreach(QAction *action, d->statusGroups.value(d->dockMenu)->actions())
			action->setChecked(false);
		if (d->isTrayAvailable)
			foreach(QAction *action, d->statusGroups.value(d->trayMenu)->actions())
				action->setChecked(false);
	}
}

void MacDock::loadSettings()
{
	Q_D(MacDock);
	d->isTrayAvailable = Config("macosx").group("dock").value("showtray", true);
	if (d->isTrayAvailable)
		createTrayDeps();
	else if (d->tray)
		removeTrayDeps();
}
}

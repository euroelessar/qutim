/****************************************************************************
*  chatsessionimpl.cpp
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

#include "chatsessionimpl.h"
#include <QStringBuilder>
#include "chatlayerimpl.h"
#include "chatsessionmodel.h"
#include <QApplication>
#include <QPlainTextDocumentLayout>
#include "chatsessionimpl_p.h"
#include "chatforms/abstractchatform.h"
#include <qutim/message.h>
#include <qutim/account.h>
#include <qutim/notification.h>
#include <qutim/conference.h>
#include <qutim/debug.h>
#include <qutim/servicemanager.h>
#include "chatviewfactory.h"

namespace Core
{
namespace AdiumChat
{

ChatSessionImplPrivate::ChatSessionImplPrivate() :
	controller(0),
	myself_chat_state(ChatStateInActive)
{
}

ChatSessionImplPrivate::~ChatSessionImplPrivate()
{
}

ChatSessionImpl::ChatSessionImpl(ChatUnit* unit, ChatLayer* chat)
	: ChatSession(chat),
	d_ptr(new ChatSessionImplPrivate)
{
	Q_D(ChatSessionImpl);
	d->input = new QTextDocument(this);
	d->model = new ChatSessionModel(this);
	d->q_ptr = this;
	d->chatUnit = unit;
	d->input->setDocumentLayout(new QPlainTextDocumentLayout(d->input));
	Config cfg = Config("appearance").group("chat");
	d->sendToLastActiveResource = cfg.value("sendToLastActiveResource", false);
	d->active = false;
	d->inactive_timer.setSingleShot(true);

	connect(&d->inactive_timer,SIGNAL(timeout()),d,SLOT(onActiveTimeout()));
	d->chatUnit = 0;
	setChatUnit(unit);
}

void ChatSessionImpl::clearChat()
{
	Q_D(ChatSessionImpl);
	d->getController()->clearChat();
}

bool ChatSessionImpl::hasJavaScript() const {
	const QMetaObject *meta = getController()->metaObject();
	return meta->indexOfMethod("evaluateJavaScript(QString)") != -1;
}

QString ChatSessionImpl::quote()
{
	return d_func()->getController()->quote();
}

ChatSessionImpl::~ChatSessionImpl()
{
	Q_D(ChatSessionImpl);
	setChatState(ChatStateGone);
	if (d->menu)
		d->menu->deleteLater();
}

void ChatSessionImpl::addContact(Buddy* c)
{
	//		connect(c,SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),SLOT(statusChanged(qutim_sdk_0_3::Status)));
	d_func()->model->addContact(c);
	emit buddiesChanged();
}

qint64 ChatSessionImpl::doAppendMessage(Message &message)
{
	Q_D(ChatSessionImpl);
	if (!message.chatUnit()) {
		qWarning() << QString("Message %1 must have a chatUnit").arg(message.text());
		message.setChatUnit(getUnit());
	}

	if (message.isIncoming())
		messageReceived(&message);
	else
		messageSent(&message);

	if (message.property("spam", false) || message.property("hide", false))
		return message.id();

	if ((!isActive() && !message.property("service", false)) && message.isIncoming()) {
		d->unread.append(message);
		unreadChanged(d->unread);
	}

	//if (!message.isIncoming())
	//	setChatState(ChatStateActive);

	bool service = message.property("service").isValid();
	const Conference *conf = qobject_cast<const Conference *>(message.chatUnit());
	if (!service && !conf
		&& message.chatUnit() != d->current_unit
		&& message.isIncoming()
		&& !message.property("history", false))
	{
		d->last_active_unit = const_cast<ChatUnit*>(message.chatUnit());
	}

	if (conf) {
		QString sender = conf->me() ? conf->me()->name() : QString();
		if (message.text().contains(sender)) {
			AbstractChatForm *form = ServiceManager::getByName<AbstractChatForm*>("ChatForm");
			if (form) {
				QWidget *widget = form->chatWidget(this);
				if (widget) {
					QApplication::alert(widget,5000);
				}
			}
		}
	}

	if (!message.property("silent", false))
		Notification::send(message);

	if(!message.property("fake",false)) {
//		qApp->postEvent(this, new MessageEventHook(message), Qt::LowEventPriority);
		d->getController()->appendMessage(message);
	}
	return message.id();
}

void ChatSessionImpl::removeContact(Buddy *c)
{
	d_func()->model->removeContact(c);
	emit buddiesChanged();
}

Account* ChatSessionImpl::getAccount() const
{
	return d_func()->chatUnit->account();
}

QString ChatSessionImpl::getId() const
{
	return d_func()->chatUnit->id();
}


ChatUnit* ChatSessionImpl::getUnit() const
{
	return d_func()->chatUnit;
}

QObject* ChatSessionImpl::getController() const
{
	Q_D(const ChatSessionImpl);
	d->ensureController();
	return d->controller;
}

ChatViewController *ChatSessionImplPrivate::getController() const
{
	ensureController();
	return qobject_cast<ChatViewController*>(controller);
}

void ChatSessionImplPrivate::ensureController() const
{
	if(!controller) {
		ChatViewFactory *factory = ServiceManager::getByName<ChatViewFactory*>("ChatViewFactory");
		controller = factory->createViewController();
		ChatViewController *c = qobject_cast<ChatViewController*>(controller);
		Q_ASSERT(c);
		c->setChatSession(q_ptr);
	}
}

ChatUnit* ChatSessionImpl::getCurrentUnit() const
{
	Q_D(const ChatSessionImpl);
	if (d->sendToLastActiveResource)
		return d->last_active_unit ? d->last_active_unit : d->chatUnit;
	else
		return d->current_unit ? d->current_unit : d->chatUnit;
}

QVariant ChatSessionImpl::evaluateJavaScript(const QString &scriptSource)
{
	QVariant retVal;
	QMetaObject::invokeMethod(getController(),
							  "evaluateJavaScript",
							  Q_RETURN_ARG(QVariant,retVal),
							  Q_ARG(QString,scriptSource));
	return retVal;
}

void ChatSessionImpl::setActive(bool active)
{
	Q_D(ChatSessionImpl);
	if (d->active == active)
		return;
	if (active)
		setChatState(ChatStateActive);
	else if (d->myself_chat_state != ChatStateGone)
		setChatState(ChatStateInActive);
	d->active = active;
	emit activated(active);
}

bool ChatSessionImpl::isActive()
{
	return d_func()->active;
}

bool ChatSessionImpl::event(QEvent *ev)
{
//	if (ev->type() == MessageEventHook::eventType()) {
//		MessageEventHook *messageEvent = static_cast<MessageEventHook*>(ev);
//		qDebug() << Q_FUNC_INFO;
//		d_func()->getController()->appendMessage(messageEvent->message);
//		return true;
//	}
	return ChatSession::event(ev);
}

QAbstractItemModel* ChatSessionImpl::getModel() const
{
	return d_func()->model;
}

//ChatState ChatSessionImplPrivate::statusToState(Status::Type type)
//{
//	//TODO may be need to move to protocols?
//	switch(type) {
//		case Status::Offline: {
//			return ChatStateGone;
//			break;
//		}
//		case Status::NA: {
//			return ChatStateInActive;
//			break;
//		}
//		case Status::Away: {
//			return ChatStateInActive;
//			break;
//		}
//		case Status::DND: {
//			return ChatStateInActive;
//			break;
//		}
//		case Status::Online: {
//			return ChatStateActive;
//			break;
//		}
//		default: {
//			break;
//		}
//	}
//	//It is a good day to die!
//	return ChatStateActive;
//}

QTextDocument* ChatSessionImpl::getInputField()
{
	return d_func()->input;
}

void ChatSessionImpl::markRead(quint64 id)
{
	Q_D(ChatSessionImpl);
	if (id == Q_UINT64_C(0xffffffffffffffff)) {
		d->unread.clear();
		unreadChanged(d->unread);
		return;
	}
	MessageList::iterator it = d->unread.begin();
	for (; it != d->unread.end(); it++) {
		if (it->id() == id) {
			d->unread.erase(it);
			unreadChanged(d->unread);
			return;
		}
	}
}

MessageList ChatSessionImpl::unread() const
{
	return d_func()->unread;
}

void ChatSessionImpl::setChatUnit(ChatUnit* unit)
{
	Q_D(ChatSessionImpl);
	if (d->chatUnit)
		disconnect(d->chatUnit, 0, this, 0);
	ChatUnit *oldUnit = d->chatUnit;
	static_cast<ChatLayerImpl*>(ChatLayer::instance())->onUnitChanged(oldUnit, unit);
	d->chatUnit = unit;
	connect(unit,SIGNAL(destroyed(QObject*)),SLOT(deleteLater()));
	setParent(unit);

	Conference *conf;
	if (!!(conf = qobject_cast<Conference *>(oldUnit))) {
		foreach (ChatUnit *u, conf->lowerUnits()) {
			if (Buddy *buddy = qobject_cast<Buddy*>(u))
				removeContact(buddy);
		}
	}
	if (!!(conf = qobject_cast<Conference *>(unit))) {
		foreach (ChatUnit *u, conf->lowerUnits()) {
			if (Buddy *buddy = qobject_cast<Buddy*>(u))
				addContact(buddy);
		}
	}

	if (d->menu)
		d->refillMenu();

	emit chatUnitChanged(unit);
}

void ChatSessionImplPrivate::onActiveTimeout()
{
	Q_Q(ChatSessionImpl);
	switch(myself_chat_state) {
		case ChatStateComposing:
			q->setChatState(ChatStatePaused);
			break;
		case ChatStatePaused:
			q->setChatState(ChatStateActive);
			break;
		case ChatStateActive:
			q->setChatState(ChatStateInActive);
			break;
		case ChatStateInActive:
			q->setChatState(ChatStateGone);
			break;
		default:
			break;
	}
}

void ChatSessionImpl::setChatState(ChatState state)
{
	Q_D(ChatSessionImpl);
	if(d->myself_chat_state == state) {
		d->inactive_timer.start();
		return;
	}
	if (ChatUnit *currentUnit = getCurrentUnit()) {
		ChatStateEvent event(state);
		qApp->sendEvent(currentUnit, &event);
	}
	d->myself_chat_state = state;
	switch(state) {
		case ChatStateComposing:
		// By xep-0085 this time should be 30 secs, but it's too huge
			d->inactive_timer.setInterval(10000);
			break;
		case ChatStatePaused:
			d->inactive_timer.setInterval(30000);
			break;
		case ChatStateActive:
			d->inactive_timer.setInterval(120000);
			break;
		case ChatStateInActive:
			d->inactive_timer.setInterval(600000);
			break;
		default:
			break;
	}
	d->inactive_timer.start();
}


void ChatSessionImplPrivate::onResourceChosen(bool active)
{
	if (!active)
		return;
	Q_ASSERT(qobject_cast<QAction*>(sender()));
	QAction *action = reinterpret_cast<QAction*>(sender());
	current_unit = qVariantValue<ChatUnit*>(action->data());
}

void ChatSessionImplPrivate::onSendToLastActiveResourceActivated(bool active)
{
	sendToLastActiveResource = active;
}

void ChatSessionImplPrivate::onLowerUnitAdded()
{
	if (!menu)
		return;
	if (menu->isVisible())
		connect(menu.data(), SIGNAL(aboutToHide()), SLOT(refillMenu()));
	else
		refillMenu();
}

void ChatSessionImplPrivate::refillMenu()
{
	Q_Q(ChatSessionImpl);
	if (menu) {
		qDeleteAll(group->actions());
		ChatUnit *unit = chatUnit;
		fillMenu(menu, unit, unit->lowerUnits());
	} else {
		Q_UNUSED(q->menu());
	}
}

void ChatSessionImplPrivate::fillMenu(QMenu *menu, ChatUnit *unit, const ChatUnitList &lowerUnits, bool root)
{
	Q_Q(ChatSessionImpl);
	QAction *act = new QAction(menu);
	act->setText(QT_TRANSLATE_NOOP("ChatSession", "Auto"));
	act->setData(qVariantFromValue(unit));
	act->setCheckable(true);
	act->setChecked(!sendToLastActiveResource && unit == q->getCurrentUnit());
	group->addAction(act);
	connect(act, SIGNAL(toggled(bool)), SLOT(onResourceChosen(bool)));
	menu->addAction(act);

	if (root) {
		act = new QAction(menu);
		act->setText(QT_TRANSLATE_NOOP("ChatSession", "Last active"));
		act->setCheckable(true);
		act->setChecked(sendToLastActiveResource);
		group->addAction(act);
		connect(act, SIGNAL(toggled(bool)), SLOT(onSendToLastActiveResourceActivated(bool)));
		menu->addAction(act);
	}

	menu->addSeparator();

	foreach (ChatUnit *lower, lowerUnits) {
		connect(lower, SIGNAL(lowerUnitAdded(ChatUnit*)), SLOT(onLowerUnitAdded()));
		ChatUnitList lowerLowerUnits = lower->lowerUnits();
		if (lowerLowerUnits.isEmpty()) {
			// That unit does not have any lower units
			// so just create an action for it.
			act = new QAction(menu);
			act->setText(lower->title());
			act->setData(qVariantFromValue(lower));
			act->setCheckable(true);
			act->setChecked(!sendToLastActiveResource && lower == q->getCurrentUnit());
			group->addAction(act);
			menu->addAction(act);
			connect(lower, SIGNAL(destroyed()), act, SLOT(deleteLater()));
			connect(act, SIGNAL(toggled(bool)), SLOT(onResourceChosen(bool)));
		} else {
			// That unit has lower units and we need to create a submenu for it.
			QMenu *submenu = new QMenu(lower->title(), menu);
			fillMenu(submenu, lower, lowerLowerUnits, false);
			menu->addMenu(submenu);
			connect(lower, SIGNAL(destroyed()), submenu, SLOT(deleteLater()));
		}
	}
}

QMenu *ChatSessionImpl::menu()
{
	Q_D(ChatSessionImpl);
	//for JMessageSession
	//FIXME maybe need to move to the protocols
	//ChatUnit *unit = const_cast<ChatUnit*>(d->chat_unit->getHistoryUnit());
	ChatUnit *unit = d->chatUnit;
	if (!d->menu && qobject_cast<Conference*>(unit) == 0) {
		d->menu = new QMenu();
		if (!d->group) {
			d->group = new QActionGroup(d->menu);
			d->group->setExclusive(true);
		}
		d->fillMenu(d->menu, unit, unit->lowerUnits());
		connect(unit, SIGNAL(lowerUnitAdded(ChatUnit*)), d, SLOT(onLowerUnitAdded()));
	}
	return d->menu;
}

ChatState ChatSessionImpl::getChatState() const
{
	return d_func()->myself_chat_state;
}
}
}

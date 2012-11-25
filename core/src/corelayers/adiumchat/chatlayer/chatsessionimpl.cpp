/****************************************************************************
**
** qutIM - instant messenger
**
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

#include "chatsessionimpl.h"
#include <QStringBuilder>
#include "chatlayerimpl.h"
#include "chatsessionmodel.h"
#include <QApplication>
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

enum { LastMessagesCount = 5 };

ChatSessionImplPrivate::ChatSessionImplPrivate() :
	hasJavaScript(false),
	focus(InFocus),
	myselfChatState(ChatStateInActive),
	m_uid2menu(false)
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
	d->lastMessagesIndex = 0;
	Config cfg = Config("appearance").group("chat");
	d->sendToLastActiveResource = cfg.value("sendToLastActiveResource", false);
	d->inactive_timer.setSingleShot(true);

	connect(&d->inactive_timer,SIGNAL(timeout()),d,SLOT(onActiveTimeout()));
	d->chatUnit.clear();
	setChatUnit(unit);
}

void ChatSessionImpl::clearChat()
{
	Q_D(ChatSessionImpl);
	d->getController()->clearChat();
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
		d->menu.data()->deleteLater();
}

void ChatSessionImpl::addContact(Buddy* c)
{
	//		connect(c,SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),SLOT(statusChanged(qutim_sdk_0_3::Status)));
	d_func()->model.data()->addContact(c);
	emit buddiesChanged();
}

qint64 ChatSessionImpl::doAppendMessage(Message &message)
{
	Q_D(ChatSessionImpl);
	if (!message.chatUnit()) {
		warning() << "Message" << message.text() << "must have a chatUnit";
		message.setChatUnit(getUnit());
	}

	if (message.isIncoming())
		messageReceived(&message);
	else
		messageSent(&message);

	if (message.property("spam", false) || message.property("hide", false))
		return message.id();

	if ((!isActive() && !message.property("service", false))
			&& message.isIncoming()
			&& !message.property("history", false)) {
		d->unread.append(message);
		unreadChanged(d->unread);
	}

	//if (!message.isIncoming())
	//	setChatState(ChatStateActive);

	bool service = message.property("service").isValid();
	const Conference *conf = qobject_cast<const Conference *>(message.chatUnit());
	if (!service && !conf
			&& message.chatUnit() != d->current_unit.data()
			&& message.isIncoming()
			&& !message.property("history", false))
	{
		d->last_active_unit = const_cast<ChatUnit*>(message.chatUnit());
	}

	if (!message.property("service", false)
			&& (!conf || message.property("mention", false))
			&& message.isIncoming()
			&& !message.property("history", false)) {
		ChatLayer::instance()->alert(300);
		if (conf) {
			ServicePointer<AbstractChatForm> form("ChatForm");
			if (form) {
				if (QWidget *widget = form->chatWidget(this)) {
					QApplication::alert(widget, 300);
				}
			}
		}
	}

	if (!message.property("silent", false))
		Notification::send(message);

	if(!message.property("fake",false)) {
		if (d->focus == ChatSessionImplPrivate::FirstOutOfFocus)
			message.setProperty("firstFocus", true);
		if (d->focus & ChatSessionImplPrivate::OutOfFocus)
			message.setProperty("focus", true);
		d->focus &= ChatSessionImplPrivate::OutOfFocus;
		d->getController()->appendMessage(message);
		if (!message.property("service", false) && !message.property("topic", false)) {
			if (d->lastMessages.count() < LastMessagesCount) {
				d->lastMessages << message;
			} else {
				d->lastMessages[d->lastMessagesIndex] = message;
				d->lastMessagesIndex = (d->lastMessagesIndex + 1) % d->lastMessages.count();
			}
		}
	}
	return message.id();
}

void ChatSessionImpl::removeContact(Buddy *c)
{
	d_func()->model.data()->removeContact(c);
	emit buddiesChanged();
}

Account* ChatSessionImpl::getAccount() const
{
	return d_func()->chatUnit.data()->account();
}

QString ChatSessionImpl::getId() const
{
	return d_func()->chatUnit.data()->id();
}


ChatUnit* ChatSessionImpl::getUnit() const
{
	return d_func()->chatUnit.data();
}

QObject* ChatSessionImpl::controller()
{
	Q_D(ChatSessionImpl);
	d->ensureController();
	return d->controller.data();
}

QObject *ChatSessionImpl::controller() const
{
	return d_func()->controller.data();
}

ChatViewController *ChatSessionImplPrivate::getController()
{
	ensureController();
	return qobject_cast<ChatViewController*>(controller.data());
}

void ChatSessionImplPrivate::ensureController()
{
	Q_Q(ChatSessionImpl);
	if (!controller) {
		ChatViewFactory *factory = ServiceManager::getByName<ChatViewFactory*>("ChatViewFactory");
		controller = factory->createViewController();
		ChatViewController *c = qobject_cast<ChatViewController*>(controller.data());
		Q_ASSERT(c);
		c->setChatSession(q_ptr);
		hasJavaScript = controller.data()->metaObject()->indexOfMethod("evaluateJavaScript(QString)") != -1;
		emit q->javaScriptSupportChanged(hasJavaScript); //hack, because getController is a const method
		connect(controller.data(), SIGNAL(destroyed(QObject*)), q, SIGNAL(controllerDestroyed(QObject*)));
	}
}

ChatUnit* ChatSessionImpl::getCurrentUnit() const
{
	Q_D(const ChatSessionImpl);
	if (d->sendToLastActiveResource)
		return d->last_active_unit ? d->last_active_unit.data() : d->chatUnit.data();
	else
		return d->current_unit ? d->current_unit.data() : d->chatUnit.data();
}

MessageList ChatSessionImpl::lastMessages() const
{
	Q_D(const ChatSessionImpl);
	MessageList messages;
	for (int i = 0; i < d->lastMessages.count(); ++i) {
		int index = (i + d->lastMessagesIndex) % d->lastMessages.count();
		messages << d->lastMessages.at(index);
	}
	return messages;
}

QVariant ChatSessionImpl::evaluateJavaScript(const QString &scriptSource)
{
	QVariant retVal;
	QMetaObject::invokeMethod(controller(),
							  "evaluateJavaScript",
							  Q_RETURN_ARG(QVariant,retVal),
							  Q_ARG(QString,scriptSource));
	return retVal;
}

void ChatSessionImpl::doSetActive(bool active)
{
	Q_D(ChatSessionImpl);
	if (active) {
		setChatState(ChatStateActive);
		d->focus = ChatSessionImplPrivate::InFocus;
	} else if (d->myselfChatState != ChatStateGone) {
		setChatState(ChatStateInActive);
		d->focus = ChatSessionImplPrivate::FirstOutOfFocus;
	}
}

QAbstractItemModel* ChatSessionImpl::getModel() const
{
	return d_func()->model.data();
}

QTextDocument* ChatSessionImpl::getInputField()
{
	return d_func()->input.data();
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
		disconnect(d->chatUnit.data(), 0, this, 0);
	ChatUnit *oldUnit = d->chatUnit.data();
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
	switch(myselfChatState) {
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
	if(d->myselfChatState == state) {
		d->inactive_timer.start();
		return;
	}
	if (ChatUnit *currentUnit = getCurrentUnit()) {
		ChatStateEvent event(state);
		qApp->sendEvent(currentUnit, &event);
	}
	d->myselfChatState = state;
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
	if (menu.data()->isVisible())
		connect(menu.data(), SIGNAL(aboutToHide()), SLOT(refillMenu()));
	else
		refillMenu();
}

void ChatSessionImplPrivate::refillMenu()
{
	Q_Q(ChatSessionImpl);
	if (menu) {
		qDeleteAll(group.data()->actions());
		ChatUnit *unit = chatUnit.data();
		fillMenu(menu.data(), unit, unit->lowerUnits());
	} else {
		Q_UNUSED(q->menu(m_uid2menu));
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
	group.data()->addAction(act);
	connect(act, SIGNAL(toggled(bool)), SLOT(onResourceChosen(bool)));
	menu->addAction(act);

	if (root) {
		act = new QAction(menu);
		act->setText(QT_TRANSLATE_NOOP("ChatSession", "Last active"));
		act->setCheckable(true);
		act->setChecked(sendToLastActiveResource);
		group.data()->addAction(act);
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
			if (m_uid2menu)
				act->setText(QString("%1 (%2)").arg(lower->title(), lower->id()));
			else
				act->setText(lower->title());
			act->setData(qVariantFromValue(lower));
			act->setCheckable(true);
			act->setChecked(!sendToLastActiveResource && lower == q->getCurrentUnit());
			group.data()->addAction(act);
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

QMenu *ChatSessionImpl::menu(bool uid2menu)
{
	Q_D(ChatSessionImpl);
	//for JMessageSession
	//FIXME maybe need to move to the protocols
	//ChatUnit *unit = const_cast<ChatUnit*>(d->chat_unit->getHistoryUnit());
	ChatUnit *unit = d->chatUnit.data();
	if (!d->menu && qobject_cast<Conference*>(unit) == 0) {
		d->m_uid2menu = uid2menu;
		d->menu = new QMenu();
		if (!d->group) {
			d->group = new QActionGroup(d->menu.data());
			d->group.data()->setExclusive(true);
		}
		d->fillMenu(d->menu.data(), unit, unit->lowerUnits());
		connect(unit, SIGNAL(lowerUnitAdded(ChatUnit*)), d, SLOT(onLowerUnitAdded()));
	}
	return d->menu.data();
}

ChatState ChatSessionImpl::getChatState() const
{
	return d_func()->myselfChatState;
}

bool ChatSessionImpl::isJavaScriptSupported() const
{
	return d_func()->hasJavaScript;
}

}
}


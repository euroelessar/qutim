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
#include "chatstyleoutput.h"
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include "libqutim/message.h"
#include "libqutim/account.h"
#include <QStringBuilder>
#include <QDateTime>
#include <QDebug>
#include <libqutim/history.h>
#include <libqutim/notificationslayer.h>
#include <libqutim/conference.h>
#include "chatlayerimpl.h"
#include "chatsessionmodel.h"
#include <QApplication>
#include <libqutim/debug.h>
#include <QPlainTextDocumentLayout>
#include <QDesktopServices>
#include <QWebElement>
#include "chatsessionimpl_p.h"
#include "javascriptclient.h"
#include "chatforms/abstractchatform.h"

namespace Core
{
	namespace AdiumChat
	{
		ChatSessionImplPrivate::ChatSessionImplPrivate()
			: chat_style_output(new ChatStyleOutput),
			web_page(new QWebPage(this)),
			input(new QTextDocument),
			separator(true),
			myself_chat_state(ChatStateInActive)
		{
		}

		ChatSessionImplPrivate::~ChatSessionImplPrivate()
		{
			delete chat_style_output;
		}

		ChatSessionImpl::ChatSessionImpl ( ChatUnit* unit, ChatLayer* chat)
			: ChatSession ( chat ),
			d_ptr(new ChatSessionImplPrivate)
		{
			Q_D(ChatSessionImpl);
			d->model = new ChatSessionModel(this);
			d->q_ptr = this;
			setChatUnit(unit);
			d->input->setDocumentLayout(new QPlainTextDocumentLayout(d->input));
			qDebug() << "create session" << d->chat_unit->title();
			connect(unit,SIGNAL(destroyed(QObject*)),SLOT(deleteLater()));
			d->store_service_messages = Config("appearance").group("chat/history").value<bool>("storeServiceMessages", true);
			d->groupUntil = Config("appearance").group("chat").value<int>("groupUntil", 900);
			d->chat_style_output->preparePage(d->web_page,this);
			d->skipOneMerge = true;
			d->active = false;
			if (Conference *conf = qobject_cast<Conference *>(unit)) {
				foreach (ChatUnit *u, conf->lowerUnits()) {
					if (Buddy *buddy = qobject_cast<Buddy*>(u))
						d->model->addContact(buddy);
				}
			}

			d->loadHistory();

			if (Contact *c = qobject_cast<Contact *>(unit)) {
				d->statusChanged(c->status(),c,true);
				setProperty("currentChatState",d->statusToState(c->status().type()));
			}
			else {
				//if you create a session, it is likely that the chat state is active
				setProperty("currentChatState",static_cast<int>(ChatStateActive));
				setChatState(ChatStateActive);
			}

			d->inactive_timer.setInterval(120000);
			d->inactive_timer.setSingleShot(true);

			connect(d->web_page,SIGNAL(linkClicked(QUrl)),d,SLOT(onLinkClicked(QUrl)));
			connect(&d->inactive_timer,SIGNAL(timeout()),d,SLOT(onActiveTimeout()));
			d->web_page->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
			JavaScriptClient *client = new JavaScriptClient(this);
			d->web_page->mainFrame()->addToJavaScriptWindowObject(client->objectName(), client);
			connect(d->web_page->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()),
					client, SLOT(helperCleared()));
			bool isConference = !!qobject_cast<Conference*>(d->chat_unit);
			QWebFrame *frame = d->web_page->mainFrame();
			QWebElement chatElem = frame->findFirstElement("#Chat");
			if (!chatElem.isNull()) {
				if (isConference != chatElem.hasClass("groupchat")) {
					if (isConference)
						chatElem.addClass("groupchat");
					else
						chatElem.removeClass("groupchat");
				}
			}
		}

		void ChatSessionImpl::loadTheme(const QString& path, const QString& variant)
		{
			Q_D(ChatSessionImpl);
			d->chat_style_output->loadTheme(path,variant);
			d->chat_style_output->preparePage(d->web_page,this);
		}

		void ChatSessionImpl::setVariant(const QString& variant)
		{
			Q_D(ChatSessionImpl);
			d->chat_style_output->setVariant(variant);
			d->chat_style_output->reloadStyle(d->web_page);
		}

		QString ChatSessionImpl::getVariant() const
		{
			return d_func()->chat_style_output->getVariant();
		}

		void ChatSessionImpl::setCustomCSS(const QString &css)
		{
			Q_D(ChatSessionImpl);
			d->chat_style_output->setCustomCSS(css);
			d->chat_style_output->reloadStyle(d->web_page);
		}

		void ChatSessionImplPrivate::loadHistory()
		{
			Q_Q(ChatSessionImpl);
			ConfigGroup adium_chat = Config("appearance").group("chat/history");
			int max_num = adium_chat.value<int>("maxDisplayMessages",5);
			MessageList messages = History::instance()->read(q->getUnit(), max_num);
			foreach (Message mess, messages) {
				mess.setProperty("silent",true);
				mess.setProperty("store",false);
				mess.setProperty("history",true);
				if (!mess.chatUnit()) //TODO FIXME
					mess.setChatUnit(q->getUnit());
				q->appendMessage(mess);
			}
			previous_sender.clear();
			skipOneMerge = true;
		}

		ChatSessionImpl::~ChatSessionImpl()
		{
			Q_D(ChatSessionImpl);
			if (d->menu)
				d->menu->deleteLater();
			d->input->deleteLater();
		}

		void ChatSessionImpl::addContact(Buddy* c)
		{
			//		connect(c,SIGNAL(statusChanged(qutim_sdk_0_3::Status)),SLOT(statusChanged(qutim_sdk_0_3::Status)));
			d_func()->model->addContact(c);
			emit buddiesChanged();
		}

		qint64 ChatSessionImpl::appendMessage(Message &message)
		{
			Q_D(ChatSessionImpl);
			if (!message.chatUnit()) {
				qWarning() << QString("Message %1 must have a chatUnit").arg(message.text());
				message.setChatUnit(getUnit());
			}
			if (!isActive() && !message.property("service", false)) {
				d->unread.append(message);
				unreadChanged(d->unread);
			}

			if (message.isIncoming())
				messageReceived(&message);
			else
				messageSent(&message);

			if (message.property("spam", false) || message.property("hide", false))
				return message.id();

			if (!message.isIncoming())
				setChatState(ChatStateActive);

			bool same_from = false;
			bool service = message.property("service").isValid();
			QString item;
			if (!isActive()) {
				if (!d->separator && !message.property("service", false) && qobject_cast<const Conference *>(message.chatUnit())) {
					QString jsInactive = QString("separator = document.getElementById(\"separator\");")
							% QString("if (separator) separator.parentNode.removeChild(separator);")
							% QString("appendMessage(\"<hr id='separator'><div id='insert'></div>\");");
					d->web_page->mainFrame()->evaluateJavaScript(jsInactive);
					d->previous_sender.clear();
					d->separator = true;
				}
			}
			if(message.text().startsWith("/me ")) {
				// FIXME we shouldn't copy data there				
				QString text = message.property("html").toString();
				if (text.startsWith("/me ")) {
					message.setProperty("html",text.mid(3));
				}
				text = message.text();
				message.setText(text.mid(3));
				item = d->chat_style_output->makeAction(this,message);
				message.setText(text);
				d->previous_sender.clear();
				d->skipOneMerge = true;
			}
			else if (service) {
				item =  d->chat_style_output->makeStatus(this,message);
				d->previous_sender.clear();
				d->skipOneMerge = true;
			}
			else {
				QString currentSender;
				if (message.isIncoming()) {
					currentSender = message.property("senderName",message.chatUnit()->title());
				}
				same_from = (!d->skipOneMerge) && (d->previous_sender == currentSender);
				if (d->lastDate.isNull())
					d->lastDate = message.time();
				if (d->lastDate.secsTo(message.time()) > d->groupUntil)
					same_from = false;
				d->lastDate = message.time();
				item =  d->chat_style_output->makeMessage(this, message, same_from);
				d->previous_sender = currentSender;
				d->skipOneMerge = false;
			}

			QString jsTask = QString("append%2Message(\"%1\");").arg(
					validateCpp(item), same_from?"Next":"");

			bool silent = message.property("silent", false);

			if (const Conference *c = qobject_cast<const Conference *>(message.chatUnit())) {
				silent = true;
				QString sender = c->me() ? c->me()->name() : QString();
				if (message.text().contains(sender)) {
					AbstractChatForm *form = qobject_cast<AbstractChatForm*>(getService("ChatForm"));
					if (form) {
						QWidget *widget = form->chatWidget(this);
						if (widget) {
							widget->raise();
						}
					}
				}
			}

			if (!silent)
				Notifications::sendNotification(message);

			if (message.property("store", true) && (!service || (service && d->store_service_messages)))
				History::instance()->store(message);
			d->web_page->mainFrame()->evaluateJavaScript(jsTask);
			return message.id();
		}

		void ChatSessionImpl::removeContact(Buddy *c)
		{
			d_func()->model->removeContact(c);
			emit buddiesChanged();
		}


		QWebPage* ChatSessionImpl::getPage() const
		{
			return d_func()->web_page;
		}

		Account* ChatSessionImpl::getAccount() const
		{
			return d_func()->chat_unit->account();
		}

		QString ChatSessionImpl::getId() const
		{
			return d_func()->chat_unit->id();
		}


		ChatUnit* ChatSessionImpl::getUnit() const
		{
			return d_func()->chat_unit;
		}

		QVariant ChatSessionImpl::evaluateJavaScript(const QString &scriptSource)
		{
			Q_D(ChatSessionImpl);
			if(d->web_page.isNull())
				return QVariant();
			return d->web_page->mainFrame()->evaluateJavaScript(scriptSource);
		}

		void ChatSessionImpl::setActive(bool active)
		{
			Q_D(ChatSessionImpl);
			if (d->active == active)
				return;
			d->active = active;
			if (!active)
				d->separator = false;
			emit activated(active);
		}

		bool ChatSessionImpl::isActive()
		{
			return d_func()->active;
		}

		bool ChatSessionImpl::event(QEvent *ev)
		{
			Q_D(ChatSessionImpl);
			if (ev->type() == MessageReceiptEvent::eventType()) {
				MessageReceiptEvent *msgEvent = static_cast<MessageReceiptEvent *>(ev);
				QWebElement elem = d->web_page->mainFrame()->findFirstElement("#message" + QString::number(msgEvent->id()));
				qDebug() << elem.toPlainText();
				elem.removeClass("notDelivered");
				elem.addClass("delivered");
				return true;
			} else {
				return ChatSession::event(ev);
			}
		}

		QAbstractItemModel* ChatSessionImpl::getModel() const
		{
			return d_func()->model;
		}

		void ChatSessionImplPrivate::onStatusChanged(qutim_sdk_0_3::Status status)
		{
			Contact *contact = qobject_cast<Contact *>(sender());
			if (!contact)
				return;
			statusChanged(status,contact,contact->property("silent").toBool());
		}

		ChatState ChatSessionImplPrivate::statusToState(Status::Type type)
		{
			//TODO may be need to move to protocols?
			switch(type) {
				case Status::Offline: {
					return ChatStateGone;
					break;
				}
				case Status::NA: {
					return ChatStateInActive;
					break;
				}
				case Status::Away: {
					return ChatStateInActive;
					break;
				}
				case Status::DND: {
					return ChatStateInActive;
					break;
				}
				case Status::Online: {
					return ChatStateActive;
					break;
				}
				default: {
					break;
				}
			}
			//It is a good day to die!
			return ChatStateActive;
		}

		void ChatSessionImplPrivate::statusChanged(const Status &status,Contact* contact, bool silent)
		{
			Q_Q(ChatSessionImpl);
			Message msg;
			Notifications::Type type = Notifications::StatusChange;
			QString title = contact->status().name().toString();


			switch(status.type()) {
				case Status::Offline: {
					type = Notifications::Offline;
					break;
				}
				case Status::Online: {
					type = Notifications::Online;
					break;
				}
				default: {
					break;
				}
			}

			debug() << "chat state event sended";
			ChatStateEvent ev(statusToState(status.type()));
			qApp->sendEvent(q,&ev);

			//title = title.isEmpty() ? contact->status().name().toString() : title;

			msg.setChatUnit(contact);
			msg.setIncoming(true);
			msg.setProperty("service",type);
			msg.setProperty("title",title);
			msg.setTime(QDateTime::currentDateTime());
			msg.setText(contact->status().text());
			msg.setProperty("silent",silent);
			q->appendMessage(msg);
		}

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
			d->chat_unit = unit;
			setParent(unit);
			Contact *c = qobject_cast<Contact *>(unit);
			if (c) {
				connect(c,SIGNAL(statusChanged(qutim_sdk_0_3::Status)),d,SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
			}
		}

		void ChatSessionImplPrivate::onActiveTimeout()
		{
			debug() << "set inactive state";
			q_func()->setChatState(ChatStateInActive);
		}


		void ChatSessionImpl::setChatState(ChatState state)
		{
			Q_D(ChatSessionImpl);
			ChatStateEvent event(state);
			qApp->sendEvent(d->chat_unit,&event);
			d->myself_chat_state = state;
			if ((state != ChatStateInActive) && (state != ChatStateGone) && (state != ChatStateComposing)) {
				d->inactive_timer.start();
				debug() << "timer activated";
			}
		}

		void ChatSessionImplPrivate::onLinkClicked(const QUrl& url)
		{
			debug() << "link clicked" << url;
			QDesktopServices::openUrl(url);
		}

		QMenu *ChatSessionImpl::menu()
		{
			Q_D(ChatSessionImpl);
			if (!d->menu) {
				d->menu = new QMenu();

				//for JMessageSession
				//FIXME maybe need to move to the protocols
				ChatUnit *unit = const_cast<ChatUnit*>(d->chat_unit->getHistoryUnit());

				QAction *act = new QAction(d->menu);
				act->setText(QT_TRANSLATE_NOOP("ChatSession", "Auto"));
				act->setData(qVariantFromValue(d->chat_unit.data()));
				act->setCheckable(true);
				act->setChecked(true);

				QActionGroup *group = new QActionGroup(d->menu);
				group->setExclusive(true);
				group->addAction(act);

				d->menu->addAction(act);
				d->menu->addSeparator();

				ChatUnitList list = unit->lowerUnits();
				foreach (ChatUnit *u, list) {
					act = new QAction(d->menu);
					act->setText(u->title());
					act->setData(qVariantFromValue(u));
					act->setCheckable(true);
					act->setChecked(u == d->chat_unit);
					group->addAction(act);
					d->menu->addAction(act);
					connect(u,SIGNAL(destroyed()),act,SLOT(deleteLater()));
				}
			}
			return d->menu;
		}
		
		ChatState ChatSessionImpl::getChatState() const
		{
			return d_func()->myself_chat_state;
		}


	}
}

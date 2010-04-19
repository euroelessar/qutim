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

namespace AdiumChat

{

	ChatSessionImpl::ChatSessionImpl ( ChatUnit* unit, ChatLayer* chat)
		: ChatSession ( chat ),m_chat_style_output(new ChatStyleOutput),m_web_page(new QWebPage),m_input(new QTextDocument(this))
	{
		setChatUnit(unit);
		m_input->setDocumentLayout(new QPlainTextDocumentLayout(m_input));
		qDebug() << "create session" << m_chat_unit->title();
		connect(unit,SIGNAL(destroyed(QObject*)),SLOT(deleteLater()));
		m_store_service_messages = Config("appearance/chat").group("general/history").value<bool>("storeServiceMessages", false);
		m_chat_style_output->preparePage(m_web_page,this);
		m_skipOneMerge = true;
		m_active = false;
		m_model = new ChatSessionModel(this);
		if (!qobject_cast<const Conference *> (unit))
			loadHistory();
		if (Contact *c = qobject_cast<Contact *>(unit))
			statusChanged(c,true);
		else {
			//if you create a session, it is likely that the chat state is active
			setProperty("currentChatState",static_cast<int>(ChatStateActive));
		}

		setChatState(ChatStateActive);
		
		connect(m_web_page,SIGNAL(linkClicked(QUrl)),SLOT(onLinkClicked(QUrl)));
		m_web_page->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
	}

	void ChatSessionImpl::loadTheme(const QString& path, const QString& variant)
	{
		m_chat_style_output->loadTheme(path,variant);
		m_chat_style_output->preparePage(m_web_page,this);
	}

	void ChatSessionImpl::setVariant(const QString& variant)
	{
		m_chat_style_output->setVariant(variant);
		m_chat_style_output->reloadStyle(m_web_page);
	}

	QString ChatSessionImpl::getVariant() const
	{
		return m_chat_style_output->getVariant();
	}

	void ChatSessionImpl::setCustomCSS(const QString &css)
	{
		m_chat_style_output->setCustomCSS(css);
		m_chat_style_output->reloadStyle(m_web_page);
	}

	void ChatSessionImpl::loadHistory()
	{
		ConfigGroup adium_chat = Config("appearance/chat").group("general/history");
		int max_num = adium_chat.value<int>("maxDisplayMessages",5);
		MessageList messages = History::instance()->read(getUnit(), max_num);
		foreach (Message mess, messages) {
			mess.setProperty("history",true);
			if (!mess.chatUnit()) //TODO FIXME
				mess.setChatUnit(getUnit()); 
			appendMessage(mess);
		}
		m_previous_sender = 0;
		m_skipOneMerge = true;
	}

	ChatSessionImpl::~ChatSessionImpl()
	{
		if (m_web_page)
			m_web_page->deleteLater();
	}

	void ChatSessionImpl::addContact(Buddy* c)
	{
		//		connect(c,SIGNAL(statusChanged(qutim_sdk_0_3::Status)),SLOT(statusChanged(qutim_sdk_0_3::Status)));
		m_model->addContact(c);
		emit buddiesChanged();
	}

	qint64 ChatSessionImpl::appendMessage(Message &message)
	{
		if (!message.chatUnit()) {
			qWarning() << tr("Message %1 must have a ChatUnit").arg(message.text());
			message.setChatUnit(getUnit());
		}
		if (!isActive()) {
			m_unread.append(message);
			unreadChanged(m_unread);
		}

		if (message.isIncoming())
			messageReceived(&message);
		else
			messageSended(&message);

		if (message.property("spam", false) || message.property("hide", false))
			return message.id();
		
		if (!message.isIncoming())
			setChatState(ChatStateActive);

		bool same_from = false;
		bool service = message.property("service").isValid();
		QString item;
		if(message.text().startsWith("/me ")) {
			// FIXME we shouldn't copy data there
			QString text = message.text();
			message.setText(text.mid(3));
			item = m_chat_style_output->makeAction(this,message);
			message.setText(text);
			m_previous_sender = 0;
			m_skipOneMerge = true;
		}
		else if (service) {
			item = m_chat_style_output->makeStatus(this,message);
			m_previous_sender = 0;
			m_skipOneMerge = true;
		}
		else {
			const ChatUnit *currentSender = message.isIncoming() ? message.chatUnit() : 0;
			same_from = (!m_skipOneMerge) && (m_previous_sender == currentSender);
			item = m_chat_style_output->makeMessage(this, message, same_from);
			m_previous_sender = currentSender;
			m_skipOneMerge = false;
		}

		QString result = m_web_page->mainFrame()->evaluateJavaScript(QString("getEditedHtml(\"%1\", \"%2\");")
																	 .arg(validateCpp(item))
																	 .arg(message.id())).toString();
		QString jsTask = QString("append%2Message(\"%1\");").arg(
				result.isEmpty() ? item :
				validateCpp(result), same_from?"Next":"");
		bool isHistory = message.property("history", false);
		bool silent = message.property("silent", false);
		if (!isHistory && !silent) {
			Notifications::sendNotification(message);
		}
		if (message.property("store", true) && (!service || (service && m_store_service_messages)))
			History::instance()->store(message);
		m_web_page->mainFrame()->evaluateJavaScript(jsTask);
		return message.id();
	}

	void ChatSessionImpl::removeContact(Buddy *c)
	{
		m_model->removeContact(c);
		emit buddiesChanged();
	}


	QWebPage* ChatSessionImpl::getPage() const
	{
		return m_web_page;
	}

	Account* ChatSessionImpl::getAccount() const
	{
		return m_chat_unit->account();
	}

	QString ChatSessionImpl::getId() const
	{
		return m_chat_unit->id();
	}


	ChatUnit* ChatSessionImpl::getUnit() const
	{
		return m_chat_unit;
	}

	QVariant ChatSessionImpl::evaluateJavaScript(const QString &scriptSource)
	{
		if(m_web_page.isNull())
			return QVariant();
		return m_web_page->mainFrame()->evaluateJavaScript(scriptSource);
	}

	void ChatSessionImpl::setActive(bool active)
	{
		if (m_active == active)
			return;
		m_active = active;
		emit activated(active);
	}

	bool ChatSessionImpl::isActive()
	{
		return m_active;
	}

	bool ChatSessionImpl::event(QEvent *ev)
	{
		if (ev->type() == MessageReceiptEvent::eventType()) {
			MessageReceiptEvent *msgEvent = static_cast<MessageReceiptEvent *>(ev);
			m_web_page->mainFrame()->evaluateJavaScript(QLatin1Literal("messageDlvrd(\"")
														% QString::number(msgEvent->id())
														% QLatin1Literal("\");"));
			return true;
		} else {
			return ChatSession::event(ev);
		}
	}

	QAbstractItemModel* ChatSessionImpl::getModel() const
	{
		return m_model;
	}
	
	void ChatSessionImpl::onStatusChanged(qutim_sdk_0_3::Status status)
	{
		Contact *contact = qobject_cast<Contact *>(sender());
		if (!contact)
			return;
		statusChanged(contact,contact->property("silent").toBool());
	}
	
	void ChatSessionImpl::statusChanged(Contact* contact, bool silent)
	{
		Message msg;		
		Notifications::Type type = Notifications::Online;
		QString title = contact->status().name().toString();
		
		switch (contact->status().type()) {
		case Status::Online: {
				ChatStateEvent ev (ChatStateActive);
				setProperty("currentChatState",static_cast<int>(ChatStateActive));
				qApp->sendEvent(this, &ev);
				debug() << "State active";
				break;
			}
		case Status::Offline: {
				ChatStateEvent ev (ChatStateGone);
				setProperty("currentChatState",static_cast<int>(ChatStateGone));
				qApp->sendEvent(this, &ev);
				type = Notifications::Offline;
				break;
			}
		default:
			type = Notifications::StatusChange;
			//title = contact->status().property("title", QVariant()).toString();
			break;
		}
		
		//title = title.isEmpty() ? contact->status().name().toString() : title;
		
		msg.setChatUnit(contact);
		msg.setIncoming(true);
		msg.setProperty("service",type);
		msg.setProperty("title",title);
		msg.setTime(QDateTime::currentDateTime());
		msg.setText(contact->status().text());
		msg.setProperty("silent",silent);
		appendMessage(msg);
	}
	
	QTextDocument* ChatSessionImpl::getInputField()
	{
		return m_input;
	}

	void ChatSessionImpl::markRead(quint64 id)
	{
		if (id == Q_UINT64_C(0xffffffffffffffff)) {
			m_unread.clear();
			unreadChanged(m_unread);
			return;
		}
		MessageList::iterator it = m_unread.begin();
		for (; it != m_unread.end(); it++) {
			if (it->id() == id) {
				m_unread.erase(it);
				unreadChanged(m_unread);
				return;
			}
		}
	}

	MessageList ChatSessionImpl::unread() const
	{
		return m_unread;
	}

	void ChatSessionImpl::setChatUnit(ChatUnit* unit)
	{
		m_chat_unit = unit;
		Contact *c = qobject_cast<Contact *>(unit);
		if (c) {
			connect(c,SIGNAL(statusChanged(qutim_sdk_0_3::Status)),SLOT(onStatusChanged(qutim_sdk_0_3::Status)));
		}
	}

	void ChatSessionImpl::timerEvent(QTimerEvent *event)
	{
		if (event->timerId() == m_inactive_timer) {
			debug() << "set inactive state";
			setChatState(ChatStateInActive);
			killTimer(m_inactive_timer);
		}
		QObject::timerEvent(event);
	}

	void ChatSessionImpl::setChatState(ChatState state)
	{
		ChatStateEvent event(state);
		qApp->sendEvent(m_chat_unit,&event);
		m_myself_chat_state = state;
		if ((state != ChatStateInActive) && (state != ChatStateGone) && (state != ChatStateComposing)) {
			killTimer(m_inactive_timer);
			m_inactive_timer = startTimer(120000);
			debug() << "timer activated";
		}
	}
	
	void ChatSessionImpl::onLinkClicked(const QUrl& url)
	{
		debug() << "link clicked" << url;
		QDesktopServices::openUrl(url);
	}

	QMenu *ChatSessionImpl::menu()
	{
		if (!m_menu) {
			m_menu = new QMenu();


			//for JMessageSession
			ChatUnit *unit = const_cast<ChatUnit*>(m_chat_unit->getHistoryUnit());

			QAction *act = new QAction(m_menu);
			act->setText(QT_TRANSLATE_NOOP("ChatSession", "Auto"));
			act->setData(qVariantFromValue(unit));
			act->setCheckable(true);
			act->setChecked(true);

			QActionGroup *group = new QActionGroup(m_menu);
			group->setExclusive(true);
			group->addAction(act);

			m_menu->addAction(act);
			m_menu->addSeparator();

			ChatUnitList list = unit->lowerUnits();
			for (int index=0;index!=list.count();index++) {
				act = new QAction(m_menu);
				act->setText(list.at(index)->title());
				act->setData(qVariantFromValue(list.at(index)));
				act->setCheckable(true);
				act->setChecked(list.at(index) == m_chat_unit);
				group->addAction(act);
				m_menu->addAction(act);
			}
		}
		return m_menu;
	}

}

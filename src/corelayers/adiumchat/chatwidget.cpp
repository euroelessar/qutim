/****************************************************************************
 *  chatwidget.cpp
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

#include "chatwidget.h"
#include "chatsessionimpl.h"
#include "ui_chatwidget.h"
#include <libqutim/account.h>
#include <libqutim/icon.h>
#include <libqutim/menucontroller.h>
#include <libqutim/debug.h>
#include <QWebFrame>
#include <QTime>
#include <libqutim/qtwin.h>

namespace AdiumChat
{
	ChatWidget::ChatWidget(bool removeSessionOnClose): 
	ui(new Ui::AdiumChatForm),m_remove_session_on_close(removeSessionOnClose)
	{
		m_current_index = -1;
		
		ui->setupUi(this);
		centerizeWidget(this);
		//init tabbar
		ui->tabBar->setVisible(false);
		ui->tabBar->setTabsClosable(true);
		ui->tabBar->setMovable(true);
		ui->tabBar->setDocumentMode(true);
		ui->tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
		ui->contactsView->hide();
		//ui->tabBar->setDrawBase(false);
		//init status and menubar
		ui->statusbar->setVisible(false);
		ui->menubar->setVisible(false);
		setAttribute(Qt::WA_DeleteOnClose);
		setAttribute(Qt::WA_MacBrushedMetal);
		
		connect(ui->tabBar,SIGNAL(currentChanged(int)),SLOT(currentIndexChanged(int)));
		connect(ui->tabBar,SIGNAL(tabMoved(int,int)),SLOT(onTabMoved(int,int)));
		connect(ui->tabBar,SIGNAL(tabCloseRequested(int)),SLOT(onCloseRequested(int)));
		connect(ui->tabBar,SIGNAL(customContextMenuRequested(QPoint)),SLOT(onTabContextMenu(QPoint)));
		connect(ui->pushButton,SIGNAL(clicked(bool)),SLOT(onSendButtonClicked()));
		
		ui->chatEdit->installEventFilter(this);
		ui->chatEdit->setFocusPolicy(Qt::StrongFocus);

		//init toolbar
		ui->actionToolBar->setStyleSheet("QToolBar{background:none;border:none}");
		ui->actionToolBar->setIconSize(QSize(16,16));
		
			//for testing
		QAction *test_act1 = new QAction(Icon("applications-internet"),tr("Testing action"),this);
		ui->actionToolBar->addAction(test_act1);
		QAction *test_act2 = new QAction(Icon("preferences-system"),tr("Testing action"),this);
		ui->actionToolBar->addAction(test_act2);		
		//
		//load settings
		m_html_message = Config("appearance/adiumChat").group("behavior/widget").value<bool>("htmlMessage",false);
		ConfigGroup adium_chat = Config("appearance/adiumChat").group("behavior/widget");
		m_chat_flags = static_cast<ChatFlag> (adium_chat.value<int>("widgetFlags",ChatStateIconsOnTabs | SendTypingNotification | ShowUnreadMessages));
		
		if (m_chat_flags & SendTypingNotification) {
			connect(ui->chatEdit,SIGNAL(textChanged()),SLOT(onTextChanged()));
			m_chatstate = ChatStateActive;
			m_timeout = 5000;
		}
		//init aero integration for win		
		if (m_chat_flags & AeroThemeIntegration) {
			if (QtWin::isCompositionEnabled()) {
				QtWin::extendFrameIntoClientArea(this);
				setContentsMargins(0, 0, 0, 0);
			}
		}
	}

	ChatWidget::~ChatWidget()
	{
		clear();
	}

	void ChatWidget::addSession(ChatSessionImpl* session)
	{
		if(m_sessions.contains(session))
			return;
		m_sessions.append(session);
		session->installEventFilter(this);
		connect(session, SIGNAL(messageReceived(Message)), SLOT(onMessageReceived(Message)));
		connect(session, SIGNAL(messageSended(Message)), SLOT(onMessageSended(Message)));
		connect(session, SIGNAL(destroyed(QObject*)), SLOT(onSessionDestroyed(QObject*)));
		connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),
				SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
		QIcon icon;
		if (m_chat_flags & AvatarsOnTabs) {
			QString imagePath = session->getUnit()->property("avatar").toString();
			icon = imagePath.isEmpty() ? Icon("user-online") : QIcon(imagePath);
		}
		if (m_chat_flags & ChatStateIconsOnTabs) {
			icon = iconForState(ChatStateActive); //FIXME
		}

		ui->tabBar->addTab(icon,session->getUnit()->title());
		if (ui->tabBar->count() >1)
			ui->tabBar->setVisible(true);
	}

	void ChatWidget::addSession(const ChatSessionList &sessions)
	{
		for (int i = 0; i!=sessions.count(); i++)
			addSession(sessions.at(i));
	}

	void ChatWidget::currentIndexChanged(int index)
	{
		if (index == -1)
			return;
		int previous_index = m_current_index;
		ChatSessionImpl *session = m_sessions.at(index);
		if ((previous_index != -1) && (previous_index != index)) {
			m_sessions.at(previous_index)->setActive(false);
			session->activate();
		}
		m_current_index = index;
		ui->contactsView->setModel(session->getModel());
		ui->contactsView->setVisible(session->getModel()->rowCount(QModelIndex()) > 0);
		if (ui->chatView->page() != session->getPage()) {
			ui->chatView->page()->setView(0);
			ui->chatView->setPage(session->getPage());
			session->getPage()->setView(ui->chatView);
		}
		setWindowTitle(tr("Chat with %1").arg(session->getUnit()->title()));
		
		//m_main_toolbar->setData(session->getUnit());
// 		if (QAbstractItemModel *model = session->getItemsModel())
// 			ui->membersView->setModel(model);

		if ((m_chat_flags & SendTypingNotification) && (m_chatstate & ChatStateComposing)) {
			killTimer(m_timerid);
			m_chatstate = ui->chatEdit->document()->isEmpty() ? ChatStateActive : ChatStatePaused;
			m_sessions.at(previous_index)->getUnit()->setChatState(m_chatstate);
		}

	}

	void ChatWidget::clear()
	{
		int count = m_sessions.count();
		for (int i = 0;i!=count;i++)
			ui->tabBar->removeTab(i);
		if (m_remove_session_on_close)
			qDeleteAll(m_sessions);
		m_sessions.clear();		
	}

	void ChatWidget::removeSession(ChatSessionImpl* session)
	{
		int index = m_sessions.indexOf(session);
		if (index == -1)
			return;
		ui->tabBar->removeTab(index);
		m_sessions.removeAt(index);
		session->disconnect(this);

		currentIndexChanged(ui->tabBar->currentIndex());

		if (ui->tabBar->count() == 1)
			ui->tabBar->setVisible(false);
		if (session && m_remove_session_on_close) {			
			session->deleteLater();
			debug () << "session removed" << index;
		}
	}

	void ChatWidget::onSessionDestroyed(QObject* object)
	{
		ChatSessionImpl *sess = static_cast<ChatSessionImpl *>(object);
		if (!sess)
			return;
		removeSession(sess);
	}

	ChatSessionList ChatWidget::getSessionList() const
	{
		return m_sessions;
	}

	void ChatWidget::onCloseRequested(int index)
	{
		removeSession(m_sessions.at(index));
	}

	void ChatWidget::onTabMoved(int from, int to)
	{
		m_sessions.move(from,to);
		debug() << "moved session" << from << to;
	}

	void ChatWidget::activate(ChatSessionImpl* session)
	{
		activateWindow();
		raise();
		//TODO customize support
		int index = m_sessions.indexOf(session);
		debug() << "active index" << index;
		if (ui->tabBar->currentIndex() != index)
			ui->tabBar->setCurrentIndex(index);

		if ((m_chat_flags & ShowUnreadMessages) && !session->unread().isEmpty()) {
			session->markRead();
		}		
	}

	void ChatWidget::onUnreadChanged(const qutim_sdk_0_3::MessageList &unread)
	{
		ChatSessionImpl *session = static_cast<ChatSessionImpl*>(sender());
		int index = m_sessions.indexOf(session);
		if (index < 0)
			return;
		if (unread.isEmpty()) {
			ChatState state = static_cast<ChatState>(session->property("currentChatState").toInt());
			QIcon icon = iconForState(state);
			ui->tabBar->setTabIcon(index, icon);
		} else if (m_chat_flags & ShowUnreadMessages) {
			ui->tabBar->setTabIcon(index, Icon("mail-unread-new"));
		}
	}

	bool ChatWidget::eventFilter(QObject *obj, QEvent *event)
	{
		if (obj->metaObject() == &ChatSessionImpl::staticMetaObject) {
			if (event->type() == ChatStateEvent::eventType()) {
				ChatStateEvent *chatEvent = static_cast<ChatStateEvent *>(event);
				chatStateChanged(chatEvent->chatState());
			}
		} else {
			if (event->type() == QEvent::KeyPress) {
				QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
				QString key = QString::number(keyEvent->key(), 16);
				QString modifiers = QString::number(keyEvent->modifiers(), 16);
				if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
					if (!(keyEvent->modifiers() & Qt::ControlModifier)) {
						onSendButtonClicked();
						return true;
					}
				}
			}
		}
		return QObject::eventFilter(obj, event);
	}

	bool ChatWidget::contains(ChatSessionImpl* session)
	{
		return m_sessions.contains(session);
	}

	void ChatWidget::onSendButtonClicked()
	{
		if (ui->chatEdit->toPlainText().trimmed().isEmpty() || ui->tabBar->currentIndex() < 0)
			return;
		ChatSessionImpl *session = m_sessions.at(ui->tabBar->currentIndex());
		ChatUnit *unit = session->getUnit();
		Message message(ui->chatEdit->toPlainText());
		if (m_html_message)
			message.setProperty("html",Qt::escape(message.text()));
		message.setIncoming(false);
		message.setChatUnit(unit);
		message.setTime(QDateTime::currentDateTime());
		session->appendMessage(message);
		session->getUnit()->sendMessage(message);
		ui->chatEdit->clear();
	}

	void ChatWidget::chatStateChanged(ChatState state)
	{
		//checks
		ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sender());
		if (!session)
			return;

		int index = m_sessions.indexOf(session);
		if (index == -1)
			return;
		
		if (m_chat_flags & ChatStateIconsOnTabs) {	
			QString icon_name;
			ChatState previous_state = static_cast<ChatState>(session->property("currentChatState").toInt());
			if (!(previous_state & ChatStateComposing) || (state & ChatStatePaused)) {
				if (!session->property("unreadMessages").toInt())
					ui->tabBar->setTabIcon(index,iconForState(state));
				session->setProperty("currentChatState",state);
			}
			debug() << previous_state << state << icon_name;
		}
	}

	QTextDocument *ChatWidget::getInputField()
	{
		return ui->chatEdit->document();
	}
	
	void ChatWidget::onTextChanged()
	{
		killTimer(m_timerid);
		m_timerid = startTimer(m_timeout);
		if (!(m_chatstate & ChatStateComposing)) {
			ChatUnit *unit = m_sessions.at(ui->tabBar->currentIndex())->getUnit();
			m_chatstate = ChatStateComposing;
			unit->setChatState(m_chatstate);
			debug()<< "typing to" << unit->title();
		}
	}

	void ChatWidget::timerEvent(QTimerEvent* e)
	{
		m_chatstate = ui->chatEdit->document()->isEmpty() ? ChatStateActive : ChatStatePaused;
		ChatUnit *unit = m_sessions.at(ui->tabBar->currentIndex())->getUnit();
		debug() << "paused to" << unit->title();
		unit->setChatState(m_chatstate);
		killTimer(m_timerid);
		QObject::timerEvent(e);
	}

	bool ChatWidget::event(QEvent *event)
	{
		if (event->type() == QEvent::WindowActivate
			|| event->type() == QEvent::WindowDeactivate) {
			bool active = event->type() == QEvent::WindowActivate;
			m_sessions.at(ui->tabBar->currentIndex())->setActive(active);
		}
		return QMainWindow::event(event);
	}
	
	void ChatWidget::onMessageReceived(const qutim_sdk_0_3::Message& message)
	{
		ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sender());
		debug() << "messageReceived" << message.text();
		if (!session)
			return;
		debug() << "message from" << session->getUnit()->title();
		int index = m_sessions.indexOf(session);
		if (index == -1)
			return;
	}

	void ChatWidget::onMessageSended(const qutim_sdk_0_3::Message& message)
	{
		ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sender());
		if (!session)
			return;
		
		int index = m_sessions.indexOf(session);
		if (index == -1)
			return;
		
		if (m_chat_flags & SendTypingNotification) {
			killTimer(m_timerid);
			m_chatstate = ChatStateActive;
			m_sessions.at(index)->getUnit()->setChatState(m_chatstate);
		}
	}

	void ChatWidget::onTabContextMenu(const QPoint &pos)
	{
		int index = ui->tabBar->tabAt(pos);
		if (index != -1) {
			if (MenuController *session = m_sessions.value(index)->getUnit()) {
				session->showMenu(ui->tabBar->mapToGlobal(pos));
			}
		}
	}
	
	QIcon ChatWidget::iconForState(ChatState state)
	{
		QString icon_name;
		switch (state) {
			//FIXME icon names
			case ChatStateActive:
				icon_name = "user-online";
				break;
			case ChatStateInActive:
				icon_name = "user-away";
				break;
			case ChatStateGone:
				icon_name =  "user-offline";
				break;
			case ChatStateComposing:
				icon_name = "mail-unread-new";
				break;
			case ChatStatePaused:
				icon_name = "mail-unread";
				break;
			default:
				break;
		}
		return Icon(icon_name);
	}

}


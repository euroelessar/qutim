/****************************************************************************
 *  chatwidget.cpp
 *
 *  Copyright (c) 2009 by Sidorov Aleksey <sauron@citadelspb.com>
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
#include <QWebFrame>
#include <QDebug>
#include <QTime>
#include <3rdparty/qtwin/qtwin.h>

namespace AdiumChat
{

	ChatWidget::ChatWidget(ChatFlags chatFlags): ui(new Ui::AdiumChatForm),m_chat_flags(chatFlags)
	{
		ui->setupUi(this);
		centerizeWidget(this);
		//init tabbar
		ui->tabBar->setVisible(false);
		ui->tabBar->setTabsClosable(true);
		ui->tabBar->setMovable(true);
		ui->tabBar->setDocumentMode(true);
		//ui->tabBar->setDrawBase(false);
		//init status and menubar
		ui->statusbar->setVisible(false);
		ui->menubar->setVisible(false);
		setAttribute(Qt::WA_DeleteOnClose);
		setAttribute(Qt::WA_MacBrushedMetal);
		connect(ui->tabBar,SIGNAL(currentChanged(int)),SLOT(currentIndexChanged(int)));
		connect(ui->tabBar,SIGNAL(tabMoved(int,int)),SLOT(onTabMoved(int,int)));
		connect(ui->tabBar,SIGNAL(tabCloseRequested(int)),SLOT(onCloseRequested(int)));
		connect(ui->pushButton,SIGNAL(clicked(bool)),SLOT(onSendButtonClicked()));
		ui->chatEdit->installEventFilter(this);
		ui->chatEdit->setFocusPolicy(Qt::StrongFocus);
		//init toolbars
		ui->additionalToolbar->setAttribute(Qt::WA_StyledBackground,false);
		ui->additionalToolbar->setBackgroundRole(QPalette::Base);
		//for testing
		QAction *test_act1 = new QAction(Icon("applications-internet"),tr("Testing action"),this);
		ui->mainToolbar->addAction(test_act1);
		QAction *test_act2 = new QAction(Icon("preferences-system"),tr("Testing action"),this);
		ui->additionalToolbar->addAction(test_act2);
		//init aero integration for win
		if (chatFlags & AeroThemeIntegration) {
			if (QtWin::isCompositionEnabled()) {
				QtWin::extendFrameIntoClientArea(this);
				setContentsMargins(0, 0, 0, 0);
			}
		}
		//
		setProperty("currentIndex",-1);
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
		connect(session,SIGNAL(removed(Account*,QString)),SLOT(onSessionRemoved()));
		connect(session,SIGNAL(remoteChatStateChanged(Contact*,ChatState)),SLOT(remoteChatStateChanged(Contact*,ChatState)));
		QIcon icon;
		if (m_chat_flags & AvatarsOnTabs) {
			QString imagePath = session->getUnit()->property("avatar").toString();
			icon = imagePath.isEmpty() ? Icon("im-user") : QIcon(imagePath);
		}
		if (m_chat_flags & ChatStateIconsOnTabs) {
			icon = Icon("im-user"); //FIXME
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
		int previous_index = property("currentIndex").toInt();
		qDebug() << "previous index" << previous_index;
		if ((previous_index != -1) && (previous_index != index)) {
			m_sessions.at(previous_index)->setActive(false);
			m_sessions.at(index)->activate();
		}
		setProperty("currentIndex",index);
		ui->chatView->setPage(m_sessions.at(index)->getPage());
		setWindowTitle(tr("Chat with %1").arg(m_sessions.at(index)->getUnit()->title()));
		//m_main_toolbar->setData(m_sessions.at(index)->getUnit());
// 		if (QAbstractItemModel *model = m_sessions.at(index)->getItemsModel())
// 			ui->membersView->setModel(model);
	}

	void ChatWidget::clear()
	{
		int count = m_sessions.count();
		for (int i = 0;i!=count;i++)
			ui->tabBar->removeTab(i);
		if (m_chat_flags & RemoveSessionOnClose)
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
		if (ui->tabBar->count() == 1)
			ui->tabBar->setVisible(false);
		currentIndexChanged(ui->tabBar->currentIndex());
		if (m_chat_flags & RemoveSessionOnClose)
			session->deleteLater();
	}

	void ChatWidget::onSessionRemoved()
	{
		ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sender());
		if (session && m_sessions.contains(session))
			removeSession(session);
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
	}

	void ChatWidget::activate(ChatSessionImpl* session)
	{
		raise();
		//TODO customize support
		int index = m_sessions.indexOf(session);
		qDebug() << "active index" << index;
		if (ui->tabBar->currentIndex() != index)
			ui->tabBar->setCurrentIndex(index);
	}

	bool ChatWidget::eventFilter(QObject *obj, QEvent *event)
	{
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
		Message message (ui->chatEdit->toPlainText());
		message.setIncoming(false);
		message.setChatUnit(session->getUnit());
		message.setTime(QDateTime::currentDateTime());
		session->getUnit()->sendMessage(message);
		session->appendMessage(message); //for testing
		ui->chatEdit->clear();
		//ugly HACK
		//ui->chatView->page()->currentFrame()->setHtml(session->getPage()->currentFrame()->toHtml());
	}

	void ChatWidget::remoteChatStateChanged(Contact *c, ChatState state)
	{
		//checks
		ChatSessionImpl *session = qobject_cast<ChatSessionImpl *>(sender());
		if (!session)
			return;
		int index = m_sessions.indexOf(session);
		if (index == -1)
			return;
		if (session->getUnit() != c) //TODO
			return;
		//states
		if (m_chat_flags & ChatStateIconsOnTabs) {
			switch (state) {
				case ChatStateActive:
					ui->tabBar->setTabIcon(index,Icon("im-user"));
					break;
				case ChatStateInActive:
					ui->tabBar->setTabIcon(index,Icon("im-user-away"));
					break;
				case ChatStateGone:
					ui->tabBar->setTabIcon(index,Icon("im-user-offline"));
					break;
				case ChatStateComposing:
					ui->tabBar->setTabIcon(index,Icon("im-user-status-message-edit"));
					break;
				case ChatStatePaused:
					ui->tabBar->setTabIcon(index,Icon("im-user-status-message-edit"));
					break;
				default:
					break;
			}
		}
	}

	QTextDocument *ChatWidget::getInputField()
	{
		return ui->chatEdit->document();
	}
}


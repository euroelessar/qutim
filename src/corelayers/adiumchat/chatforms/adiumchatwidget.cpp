/****************************************************************************
 *  AdiumChatWidget.cpp
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

#include "adiumchatwidget.h"
#include "tabbar.h"
#include "../chatsessionimpl.h"
#include <libqutim/account.h>
#include <libqutim/icon.h>
#include <libqutim/menucontroller.h>
#include <libqutim/debug.h>
#include <QWebFrame>
#include <QTime>
#include <libqutim/qtwin.h>
#include <qshortcut.h>
#include <libqutim/history.h>
#include <libqutim/conference.h>
#include "../chatsessionitemdelegate.h"
#include "ui_adiumchatwidget.h"
#include <libqutim/tooltip.h>

#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

#define MESSAGE_SOURCE_OLD            0
#define MESSAGE_SOURCE_APPLICATION    1
#define MESSAGE_SOURCE_PAGER          2
#endif //Q_WS_X11
#include <libqutim/actiontoolbar.h>

namespace Core
{
	namespace AdiumChat
	{
		AdiumChatWidget::AdiumChatWidget(const QString &widgetKey, bool removeSessionOnClose):
				AbstractChatWidget(widgetKey, removeSessionOnClose),
				ui(new Ui::AdiumChatForm)
		{

			ui->setupUi(this);
			m_originalDoc = ui->chatEdit->document();
			tabBar = new TabBar(ui->centralwidget);
			tabBar->setObjectName(QString::fromUtf8("tabBar"));
			QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Preferred);
			sizePolicy1.setHorizontalStretch(0);
			sizePolicy1.setVerticalStretch(0);
			sizePolicy1.setHeightForWidth(tabBar->sizePolicy().hasHeightForWidth());
			tabBar->setSizePolicy(sizePolicy1);
			ui->centralLayout->addWidget(tabBar);

			tabBar->setShape(QTabBar::RoundedSouth);

			tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
			ui->contactsView->hide();

			loadSettings();
			//init status and menubar

			connect(tabBar,SIGNAL(currentChanged(int)),SLOT(currentIndexChanged(int)));
			connect(tabBar,SIGNAL(tabMoved(int,int)),SLOT(onTabMoved(int,int)));
			connect(tabBar,SIGNAL(tabCloseRequested(int)),SLOT(onCloseRequested(int)));
			connect(tabBar,SIGNAL(customContextMenuRequested(QPoint)),SLOT(onTabContextMenu(QPoint)));
			ui->contactsView->setItemDelegate(new ChatSessionItemDelegate(this));

			ui->contactsView->installEventFilter(this);
			ui->chatEdit->installEventFilter(this);
			ui->chatView->installEventFilter(this);
			tabBar->installEventFilter(this);
			ui->chatEdit->setFocusPolicy(Qt::StrongFocus);

			m_toolbar = new ActionToolBar();
			addToolBar(Qt::TopToolBarArea,m_toolbar);
			m_toolbar->setMovable(false);
			m_toolbar->setMoveHookEnabled(true);
			m_toolbar->setWindowTitle(tr("Chat actions"));
			setUnifiedTitleAndToolBarOnMac(true);

			//init toolbar
			m_toolbar->setIconSize(QSize(22,22));
			//m_toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

			m_toolbar->addSeparator();
			m_before = m_toolbar->addSeparator();

			m_reciever_selector = new QAction(Icon("view-choose"),tr("Destination"),this);
			m_reciever_selector->setMenu(new QMenu()); //HACK
			m_toolbar->addAction(m_reciever_selector);
			m_reciever_selector->menu()->deleteLater();


			QWidget* spacer = new QWidget(this);
			spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			spacer->setAttribute(Qt::WA_TransparentForMouseEvents);
			// toolBar is a pointer to an existing toolbar
			m_toolbar->addWidget(spacer);

			m_session_list = new QAction(Icon("view-list-tree"),tr("Session list"),this);
			m_session_list->setMenu(new QMenu(this));
			m_toolbar->addAction(m_session_list);

			//
			//load settings
			onLoad();

			//init aero integration for win
			if (m_chatFlags & AeroThemeIntegration) {
				if (QtWin::isCompositionEnabled()) {
					m_toolbar->setStyleSheet("QToolBar{background:none;border:none;}");
					ui->centralwidget->setAutoFillBackground(true);
					QtWin::extendFrameIntoClientArea(this,
													 0,
													 0,
													 m_toolbar->sizeHint().height(),
													 0
													 );
					setContentsMargins(0, 0, 0, 0);
				}
			}
		}

		AdiumChatWidget::~AdiumChatWidget()
		{
			delete ui;
		}

		void AdiumChatWidget::loadSettings()
		{
			//init tabbar
			tabBar->setTabsClosable(true);
			tabBar->setMovable(true);
#if defined(Q_WS_MAC)
			centralWidget()->layout()->setMargin(0);
			centralWidget()->layout()->setSpacing(0);
			ConfigGroup group = Config("appearance").group("adiumChat/chatForm/adiumForm");
			bool tabUp = group.value("tabUp", false);
			if (tabUp) {
				ui->centralLayout->insertWidget(0, tabBar);
				tabBar->setDocumentMode(true);
				tabBar->setClosableActiveTab(true);
			} else {
				tabBar->setTabsClosable(false);
			}
#else
			tabBar->setMovable(true);
#endif
		}

		void AdiumChatWidget::addSession(ChatSessionImpl* session)
		{
			if(m_sessions.contains(session))
				return;
			m_sessions.append(session);
			session->installEventFilter(this);
			connect(session, SIGNAL(destroyed(QObject*)), SLOT(onSessionDestroyed(QObject*)));
			connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),
					SLOT(onUnreadChanged(qutim_sdk_0_3::MessageList)));
			connect(session, SIGNAL(buddiesChanged()), SLOT(onBuddiesChanged()));

			ChatUnit *u = session->getUnit();
			connect(u,SIGNAL(titleChanged(QString,QString)),SLOT(onUnitTitleChanged(QString)));

			QIcon icon;
			if (m_chatFlags & ChatStateIconsOnTabs) {
				ChatState state = static_cast<ChatState>(session->property("currentChatState").toInt());
				icon = ChatLayerImpl::iconForState(state,session->getUnit());
			}

			tabBar->addTab(icon,u->title());

#if defined(Q_WS_MAC)
			if (tabBar->closableActiveTab()) {
				QTabBar::ButtonPosition closeSide = (QTabBar::ButtonPosition)style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, tabBar);
				if (QWidget *button = tabBar->tabButton(tabBar->count()-1, closeSide))
					button->setVisible(false);
			}
#endif

			QAction *act = new QAction(icon,u->title(),this);
			connect(act,SIGNAL(triggered()),this,SLOT(onSessionListActionTriggered()));
			m_session_list->menu()->addAction(act);
		}

		void AdiumChatWidget::addSession(const ChatSessionList &sessions)
		{
			for (int i = 0; i!=sessions.count(); i++)
				addSession(sessions.at(i));
		}

		void AdiumChatWidget::currentIndexChanged(int index)
		{		
			if (index == -1) {
				getInputField()->setDocument(m_originalDoc);
				return;
			}
			AbstractChatWidget::currentIndexChanged(index);
			ChatSessionImpl *session = m_sessions.at(index);
			qDeleteAllLater(m_actions);
			m_actions.clear();
			ActionContainer container(session->getUnit(),ActionContainer::TypeMatch,ActionTypeChatButton);
			//fixme
			for (int i = 0;i!=container.count();i++) {
				QAction *current = container.action(i);
				m_toolbar->insertAction(m_before,current);
				m_actions.append(current);
			}
			m_reciever_selector->setMenu(session->menu());
		}

		void AdiumChatWidget::clear()
		{
			int count = m_sessions.count();
			for (int i = 0;i!=count;i++)
				tabBar->removeTab(i);
			if (m_removeSessionOnClose)
				qDeleteAll(m_sessions);
			m_sessions.clear();
		}

		void AdiumChatWidget::removeSession(ChatSessionImpl* session)
		{
			int index = m_sessions.indexOf(session);
			if (index == -1)
				return;
			m_session_list->menu()->removeAction(m_session_list->menu()->actions().at(index));
			AbstractChatWidget::removeSession(session);
		}

		void AdiumChatWidget::onSessionDestroyed(QObject* object)
		{
			ChatSessionImpl *sess = reinterpret_cast<ChatSessionImpl *>(object);
			removeSession(sess);
		}

		ChatSessionList AdiumChatWidget::getSessionList() const
		{
			return m_sessions;
		}
		
		ActionToolBar* AdiumChatWidget::getToolBar()
		{
			return m_toolbar;
		}

		void AdiumChatWidget::onCloseRequested(int index)
		{
			removeSession(m_sessions.at(index));
		}

		void AdiumChatWidget::onTabMoved(int from, int to)
		{
			m_sessions.move(from,to);

			//FIXME remove hack

			QList <QAction *> actions = m_session_list->menu()->actions();
			actions.move(from,to);
			foreach (QAction *a,m_session_list->menu()->actions())
				m_session_list->menu()->removeAction(a);
			m_session_list->menu()->addActions(actions);

			currentIndexChanged(from);

			debug() << "moved session" << from << to;
		}

		void AdiumChatWidget::activate(AdiumChat::ChatSessionImpl* session)
		{
			raise();
			activateWindow();

			//TODO customize support
			int index = m_sessions.indexOf(session);
			debug() << "active index" << index;
			if (tabBar->currentIndex() != index)
				tabBar->setCurrentIndex(index);

			if ((m_chatFlags & ShowUnreadMessages) && !session->unread().isEmpty()) {
				session->markRead();
			}

			ui->chatEdit->setFocus(Qt::ActiveWindowFocusReason);
		}

		void AdiumChatWidget::onUnreadChanged(const qutim_sdk_0_3::MessageList &unread)
		{
			ChatSessionImpl *session = static_cast<ChatSessionImpl*>(sender());
			int index = m_sessions.indexOf(session);
			if (index < 0)
				return;
			if (unread.isEmpty()) {
				ChatState state = static_cast<ChatState>(session->property("currentChatState").toInt());
				QIcon icon =  ChatLayerImpl::iconForState(state,session->getUnit());
				tabBar->setTabIcon(index, icon);
			} else if (m_chatFlags & ShowUnreadMessages) {
				tabBar->setTabIcon(index, Icon("mail-unread-new"));
			}
		}

		void AdiumChatWidget::chatStateChanged(ChatState state, ChatSessionImpl *session)
		{
			int index = m_sessions.indexOf(session);
			if (index == -1)
				return;

			if (m_chatFlags & ChatStateIconsOnTabs) {
				if (!session->unread().count()) {
					QIcon icon =  ChatLayerImpl::iconForState(state,session->getUnit());
					tabBar->setTabIcon(index,icon);
					m_session_list->menu()->actions().at(index)->setIcon(icon);
				}
			}

			session->setProperty("currentChatState",static_cast<int>(state));
		}

		QPlainTextEdit *AdiumChatWidget::getInputField()
		{
			return ui->chatEdit;
		}
		
		QTabBar *AdiumChatWidget::getTabBar()
		{
			return tabBar;
		}
		
		QListView *AdiumChatWidget::getContactsView()
		{
			return ui->contactsView;
		}

		bool AdiumChatWidget::event(QEvent *event)
		{
			if (event->type() == QEvent::WindowActivate
				|| event->type() == QEvent::WindowDeactivate) {
				bool active = event->type() == QEvent::WindowActivate;
				if (tabBar->currentIndex() == -1)
					return false;
				m_sessions.at(tabBar->currentIndex())->setActive(active);
			}
			return QMainWindow::event(event);
		}

		void AdiumChatWidget::onTabContextMenu(const QPoint &pos)
		{
			int index = tabBar->tabAt(pos);
			if (index != -1) {
				if (MenuController *session = m_sessions.value(index)->getUnit()) {
					session->showMenu(tabBar->mapToGlobal(pos));
				}
			}
		}

		void AdiumChatWidget::onSessionListActionTriggered()
		{
			QAction *act = qobject_cast<QAction *>(sender());
			Q_ASSERT(act);

			tabBar->setCurrentIndex(m_session_list->menu()->actions().indexOf(act));
		}


		void AdiumChatWidget::onUnitTitleChanged(const QString &title)
		{
			ChatUnit *u = qobject_cast<ChatUnit *>(sender());
			if (!u)
				return;
			ChatSessionImpl *s = static_cast<ChatSessionImpl *>(ChatLayerImpl::get(u,false));
			if (!s)
				return;
			tabBar->setTabText(m_sessions.indexOf(s),title);
		}
		
		QWebView* AdiumChatWidget::getChatView()
		{
			return ui->chatView;
		}

	}
}


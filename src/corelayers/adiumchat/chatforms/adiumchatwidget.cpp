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
#include "../chatsessionimpl.h"
#include <libqutim/account.h>
#include <libqutim/icon.h>
#include <libqutim/menucontroller.h>
#include <libqutim/debug.h>
#include <QWebFrame>
#include <QTime>
#include <libqutim/qtwin.h>
#include <qshortcut.h>
#include <QWidgetAction>
#include "../actions/chatemoticonswidget.h"
#include <libqutim/history.h>
#include <libqutim/shortcut.h>
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
#include "../conferencetabcompletion.h"
#include <libqutim/actiontoolbar.h>

namespace Core
{
	namespace AdiumChat
	{
		AdiumChatWidget::AdiumChatWidget(bool removeSessionOnClose):
				ui(new Ui::AdiumChatForm),m_remove_session_on_close(removeSessionOnClose)
		{
			m_current_index = -1;

			ui->setupUi(this);
			centerizeWidget(this);
			m_originalDoc = ui->chatEdit->document();

			//init tabbar
#if !defined(Q_WS_MAC)
			ui->tabBar->setTabsClosable(true);
#endif
			ui->tabBar->setMovable(true);
			//		ui->tabBar->setDocumentMode(true);
			ui->tabBar->setShape(QTabBar::RoundedSouth);
			ui->tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
			ui->contactsView->hide();
			//init status and menubar
			setAttribute(Qt::WA_DeleteOnClose);

			connect(ui->tabBar,SIGNAL(currentChanged(int)),SLOT(currentIndexChanged(int)));
			connect(ui->tabBar,SIGNAL(tabMoved(int,int)),SLOT(onTabMoved(int,int)));
			connect(ui->tabBar,SIGNAL(tabCloseRequested(int)),SLOT(onCloseRequested(int)));
			connect(ui->tabBar,SIGNAL(customContextMenuRequested(QPoint)),SLOT(onTabContextMenu(QPoint)));
			connect(ui->contactsView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
			ui->contactsView->setItemDelegate(new ChatSessionItemDelegate(this));

			ui->contactsView->installEventFilter(this);
			ui->chatEdit->installEventFilter(this);
			ui->chatView->installEventFilter(this);
			ui->tabBar->installEventFilter(this);
			ui->chatEdit->setFocusPolicy(Qt::StrongFocus);

			m_toolbar = new ActionToolBar();
			addToolBar(Qt::TopToolBarArea,m_toolbar);
			m_toolbar->setMovable(false);
			setUnifiedTitleAndToolBarOnMac(true);

			//init toolbar
			m_toolbar->setIconSize(QSize(22,22));
			//m_toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

			//for testing
			QMenu *menu = new QMenu(this);

			m_toolbar->addAction(new ActionGenerator(Icon("view-history"),
													 QT_TRANSLATE_NOOP("Chat", "View History"),
													 this,
													 SLOT(onShowHistory())));

			m_toolbar->addAction(new MenuActionGenerator(Icon("face-smile"),
														 QT_TRANSLATE_NOOP("Chat", "Emoticons"),
														 menu));
			QWidgetAction *emoticons_widget_act = new QWidgetAction(this);
			ChatEmoticonsWidget *emoticons_widget = new ChatEmoticonsWidget(this);
			emoticons_widget->loadTheme();
			emoticons_widget_act->setDefaultWidget(emoticons_widget);
			menu->addAction(emoticons_widget_act);
			connect(emoticons_widget,SIGNAL(insertSmile(QString)),ui->chatEdit,SLOT(appendPlainText(QString)));

			m_toolbar->addSeparator();

			m_reciever_selector = new QAction(Icon("view-choose"),tr("Destination"),this);
			m_reciever_selector->setMenu(new QMenu()); //HACK
			m_toolbar->addAction(m_reciever_selector);
			m_reciever_selector->menu()->deleteLater();


			QWidget* spacer = new QWidget(this);
			spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			// toolBar is a pointer to an existing toolbar
			m_toolbar->addWidget(spacer);

			m_session_list = new QAction(Icon("view-list-tree"),tr("Session list"),this);
			m_session_list->setMenu(new QMenu(this));
			m_toolbar->addAction(m_session_list);

			//
			//load settings
			m_html_message = Config("appearance/adiumChat").group("behavior/widget").value<bool>("htmlMessage",false);
			ConfigGroup adium_chat = Config("appearance/adiumChat").group("behavior/widget");
			m_chat_flags = static_cast<ChatFlag> (adium_chat.value<int>("widgetFlags",SendTypingNotification |
																		ChatStateIconsOnTabs |
																		ShowUnreadMessages |
																		SwitchDesktopOnRaise |
																		AeroThemeIntegration));

			if (m_chat_flags & SendTypingNotification) {
				connect(ui->chatEdit,SIGNAL(textChanged()),SLOT(onTextChanged()));
				m_chatstate = ChatStateActive;
				m_self_chatstate_timer.setInterval(5000);
				m_self_chatstate_timer.setSingleShot(true);
				connect(&m_self_chatstate_timer,SIGNAL(timeout()),SLOT(onChatStateTimeout()));
			}
			//init aero integration for win
			if (m_chat_flags & AeroThemeIntegration) {
				if (QtWin::isCompositionEnabled()) {
					m_toolbar->setStyleSheet("QToolBar{background:none;border:none;}");
					ui->centralwidget->setAutoFillBackground(true);
					QRect geom = m_toolbar->geometry();
					QtWin::extendFrameIntoClientArea(this,
													 0,
													 0,
													 m_toolbar->sizeHint().height(),
													 0
													 );
					setContentsMargins(0, 0, 0, 0);
				}
			}
			//init shortcuts
			Shortcut *key = new Shortcut ("chatSendMessage",this);
			connect(key,SIGNAL(activated()),this,SLOT(onSendButtonClicked()));
			key = new Shortcut ("chatCloseSession",ui->tabBar);
			connect(key,SIGNAL(activated()),SLOT(closeCurrentTab()));
			key = new Shortcut ("chatNext",ui->tabBar);
			connect(key,SIGNAL(activated()),SLOT(showNextSession()));
			key = new Shortcut ("chatPrevious",ui->tabBar);
			connect(key,SIGNAL(activated()),SLOT(showPreviousSession()));
		}

		AdiumChatWidget::~AdiumChatWidget()
		{
			if (m_remove_session_on_close) {
				foreach (ChatSessionImpl *s,m_sessions) {
					s->disconnect(this);
					s->deleteLater();
				}
			}
			delete ui;
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

			ChatUnit *u = session->getUnit();
			connect(u,SIGNAL(titleChanged(QString)),SLOT(onUnitTitleChanged(QString)));

			QIcon icon;
			if (m_chat_flags & ChatStateIconsOnTabs) {
				ChatState state = static_cast<ChatState>(session->property("currentChatState").toInt());
				icon = iconForState(state);
			}

			ui->tabBar->addTab(icon,u->title());

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
				ui->chatEdit->setDocument(m_originalDoc);
				return;
			}
			int previous_index = m_current_index;
			ChatSessionImpl *session = m_sessions.at(index);

			if ((previous_index != -1) && (previous_index != index)) {
				m_sessions.at(previous_index)->setActive(false);
				session->activate();
			}
			m_current_index = index;
			ui->contactsView->setModel(session->getModel());

			ChatUnit *u = session->getUnit();
			QIcon icon = Icon("view-choose");
			QString title = tr("Chat with %1").arg(u->title());

			if (Conference *c = qobject_cast<Conference *>(u)) {
				icon = Icon("meeting-attending"); //TODO
				title = tr("Conference %1 (%2)").arg(c->title(),c->id());
				ui->contactsView->setVisible(true);
			} else {
				ui->contactsView->setVisible(session->getModel()->rowCount(QModelIndex()) > 0);
				if (Buddy *b = qobject_cast<Buddy*>(u))
					icon = b->avatar().isEmpty() ? Icon("view-choose") : QIcon(b->avatar());
			}

			setWindowTitle(title);
			setWindowIcon(icon);

			if (ui->contactsView->isVisible()) {

				if (!m_tab_completion)
					m_tab_completion = new ConfTabCompletion(this);

				m_tab_completion->setTextEdit(ui->chatEdit);
				m_tab_completion->setChatSession(session);
			}
			else
				m_tab_completion->deleteLater();

			if (ui->chatView->page() != session->getPage()) {
				ui->chatView->page()->setView(0);
				ui->chatView->setPage(session->getPage());
				session->getPage()->setView(ui->chatView);
			}

			if ((m_chat_flags & SendTypingNotification) && (m_chatstate & ChatStateComposing)) {
				m_self_chatstate_timer.stop();
				m_chatstate = ui->chatEdit->document()->isEmpty() ? ChatStateActive : ChatStatePaused;
				m_sessions.at(previous_index)->setChatState(m_chatstate);
			}

			m_reciever_selector->setMenu(session->menu());
			ui->chatEdit->setDocument(session->getInputField());
		}

		void AdiumChatWidget::clear()
		{
			int count = m_sessions.count();
			for (int i = 0;i!=count;i++)
				ui->tabBar->removeTab(i);
			if (m_remove_session_on_close)
				qDeleteAll(m_sessions);
			m_sessions.clear();
		}

		void AdiumChatWidget::removeSession(ChatSessionImpl* session)
		{
			int index = m_sessions.indexOf(session);
			if (index == -1)
				return;
			ui->tabBar->removeTab(index);
			m_sessions.removeAt(index);
			m_session_list->menu()->removeAction(m_session_list->menu()->actions().at(index));
			session->disconnect(this);

			currentIndexChanged(ui->tabBar->currentIndex());

			if (ui->tabBar->count() == 1) {
				//ui->tabBar->setVisible(false);
				//ui->tabButton->hide();
			}
			if (session && m_remove_session_on_close) {
				session->deleteLater();
				debug () << "session removed" << index;
			}

			if (m_sessions.isEmpty())
				close();
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

			debug() << "moved session" << from << to;
		}

		void AdiumChatWidget::activate(AdiumChat::ChatSessionImpl* session)
		{
			raise();
			activateWindow();

			//TODO customize support
			int index = m_sessions.indexOf(session);
			debug() << "active index" << index;
			if (ui->tabBar->currentIndex() != index)
				ui->tabBar->setCurrentIndex(index);

			if ((m_chat_flags & ShowUnreadMessages) && !session->unread().isEmpty()) {
				session->markRead();
			}
		}

		void AdiumChatWidget::onUnreadChanged(const qutim_sdk_0_3::MessageList &unread)
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

		bool AdiumChatWidget::eventFilter(QObject *obj, QEvent *event)
		{
			if (obj == ui->contactsView) {
				if (event->type() == QEvent::ContextMenu) {
					QContextMenuEvent *menuEvent = static_cast<QContextMenuEvent*>(event);
					QModelIndex index = ui->contactsView->indexAt(menuEvent->pos());
					Buddy *buddy = index.data(Qt::UserRole).value<Buddy*>();
					if (buddy)
						buddy->showMenu(menuEvent->globalPos());
				}
			} else if (obj->metaObject() == &ChatSessionImpl::staticMetaObject) {
				if (event->type() == ChatStateEvent::eventType()) {
					ChatStateEvent *chatEvent = static_cast<ChatStateEvent *>(event);
					chatStateChanged(chatEvent->chatState(),qobject_cast<ChatSessionImpl *>(obj));
				}
			} else if (QTabBar *bar = qobject_cast<QTabBar*>(obj)){
				if (QHelpEvent *help = static_cast<QHelpEvent*>(event)) {
					if (help->type() == QEvent::ToolTip) {
						int index = bar->tabAt(help->pos());
						if (index != -1) {
							ChatUnit *unit = m_sessions.at(index)->getUnit();
							ToolTip::instance()->showText(help->globalPos(), unit, bar);
							return true;
						}
					}
				}
			}
			else {
				if (QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event)) {
					if (keyEvent->matches(QKeySequence::Copy) )
					{
						if (QWebView *view = qobject_cast<QWebView*>(obj))
							view->triggerPageAction(QWebPage::Copy);
						return true;
					}
				}
			}
			return QObject::eventFilter(obj, event);
		}

		bool AdiumChatWidget::contains(ChatSessionImpl* session)
		{
			return m_sessions.contains(session);
		}

		void AdiumChatWidget::onSendButtonClicked()
		{
			if (ui->chatEdit->toPlainText().trimmed().isEmpty() || ui->tabBar->currentIndex() < 0)
				return;
			ChatSessionImpl *session = m_sessions.at(ui->tabBar->currentIndex());
			ChatUnit *unit = 0;
			foreach (QAction *a,m_reciever_selector->menu()->actions()) {
				if (a->isChecked()) {
					unit = a->data().value<ChatUnit *>();
					break;
				}
			}
			if (!unit)
				unit = session->getUnit();

			Message message(ui->chatEdit->toPlainText());
			if (m_html_message)
				message.setProperty("html",Qt::escape(message.text()));
			message.setIncoming(false);
			message.setChatUnit(unit);
			message.setTime(QDateTime::currentDateTime());
			session->appendMessage(message);
			unit->sendMessage(message);
			ui->chatEdit->clear();

			m_self_chatstate_timer.stop();
			m_chatstate = ChatStateActive;
		}

		void AdiumChatWidget::chatStateChanged(ChatState state, ChatSessionImpl *session)
		{
			int index = m_sessions.indexOf(session);
			if (index == -1)
				return;

			if (m_chat_flags & ChatStateIconsOnTabs) {
				if (!session->unread().count()) {
					QIcon icon = iconForState(state);
					ui->tabBar->setTabIcon(index,icon);
					m_session_list->menu()->actions().at(index)->setIcon(icon);
				}
			}

			session->setProperty("currentChatState",static_cast<int>(state));
		}

		QTextDocument *AdiumChatWidget::getInputField()
		{
			return ui->chatEdit->document();
		}

		void AdiumChatWidget::onTextChanged()
		{
			m_self_chatstate_timer.stop();
			m_self_chatstate_timer.start();
			if ((m_chatstate != ChatStateComposing) && (!ui->chatEdit->toPlainText().isEmpty())) {
				m_chatstate = ChatStateComposing;
				m_sessions.at(ui->tabBar->currentIndex())->setChatState(m_chatstate);
			}
		}

		void AdiumChatWidget::onChatStateTimeout()
		{
			m_chatstate = ui->chatEdit->document()->isEmpty() ? ChatStateActive : ChatStatePaused;
			m_sessions.at(ui->tabBar->currentIndex())->setChatState(m_chatstate);
		}

		bool AdiumChatWidget::event(QEvent *event)
		{
			if (event->type() == QEvent::WindowActivate
				|| event->type() == QEvent::WindowDeactivate) {
				bool active = event->type() == QEvent::WindowActivate;
				if (ui->tabBar->currentIndex() == -1)
					return false;
				m_sessions.at(ui->tabBar->currentIndex())->setActive(active);
			}
			return QMainWindow::event(event);
		}

		void AdiumChatWidget::onTabContextMenu(const QPoint &pos)
		{
			int index = ui->tabBar->tabAt(pos);
			if (index != -1) {
				if (MenuController *session = m_sessions.value(index)->getUnit()) {
					session->showMenu(ui->tabBar->mapToGlobal(pos));
				}
			}
		}

		QIcon AdiumChatWidget::iconForState(ChatState state)
		{
			QString icon_name;
			switch (state) {
				//FIXME icon names
			case ChatStateActive:
				icon_name = "im-user";
				break;
			case ChatStateInActive:
				icon_name = "im-user-away";
				break;
			case ChatStateGone:
				icon_name =  "im-user-offline";
				break;
			case ChatStateComposing:
				icon_name = "im-status-message-edit";
				break;
			case ChatStatePaused:
				icon_name = "mail-unread";
				break;
			default:
				break;
			}
			return Icon(icon_name);
		}

		void AdiumChatWidget::closeCurrentTab()
		{
			if (ui->tabBar->count() > 1)
				removeSession(m_sessions.at(ui->tabBar->currentIndex()));
			else
				close();
		}


		void AdiumChatWidget::onShowHistory()
		{
			ChatUnit *unit = m_sessions.at(m_current_index)->getUnit();
			History::instance()->showHistory(unit);
		}

		void AdiumChatWidget::onSessionListActionTriggered()
		{
			QAction *act = qobject_cast<QAction *>(sender());
			Q_ASSERT(act);

			ui->tabBar->setCurrentIndex(m_session_list->menu()->actions().indexOf(act));
		}

		void AdiumChatWidget::showNextSession()
		{
			m_current_index++;
			if (m_current_index >= m_sessions.count())
				m_current_index = 0;
			activate(m_sessions.at(m_current_index));
		}

		void AdiumChatWidget::showPreviousSession()
		{
			m_current_index--;
			if (m_current_index < 0 )
				m_current_index = m_sessions.count() - 1;
			activate(m_sessions.at(m_current_index));
		}

		void AdiumChatWidget::onDoubleClicked(const QModelIndex &index)
		{
			Buddy *buddy = index.data(Qt::UserRole).value<Buddy*>();
			if (buddy)
				ChatLayer::get(buddy, true)->activate();
		}

		void AdiumChatWidget::onBuddiesChanged()
		{
			ChatSessionImpl *s = qobject_cast<ChatSessionImpl *>(sender());

			if (!s || (m_sessions.indexOf(s) != m_current_index))
				return;

			if (qobject_cast<Conference*>(s->getUnit()))
				ui->contactsView->setVisible(true);
			else
				ui->contactsView->setVisible(s->getModel()->rowCount(QModelIndex()) > 0);

		}

		void AdiumChatWidget::onUnitTitleChanged(const QString &title)
		{
			ChatUnit *u = qobject_cast<ChatUnit *>(sender());
			if (!u)
				return;
			ChatSessionImpl *s = static_cast<ChatSessionImpl *>(ChatLayerImpl::get(u,false));
			if (!s)
				return;
			ui->tabBar->setTabText(m_sessions.indexOf(s),title);
		}

		void AdiumChatWidget::raise()
		{
#ifdef Q_WS_X11
			if (m_chat_flags & SwitchDesktopOnRaise) {
				static Atom         NET_ACTIVE_WINDOW = 0;
				XClientMessageEvent xev;

				if(NET_ACTIVE_WINDOW == 0)
				{
					Display *dpy      = QX11Info::display();
					NET_ACTIVE_WINDOW = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
				}

				xev.type         = ClientMessage;
				xev.window       = winId();
				xev.message_type = NET_ACTIVE_WINDOW;
				xev.format       = 32;
				xev.data.l[0]    = MESSAGE_SOURCE_PAGER;
				xev.data.l[1]    = QX11Info::appUserTime();
				xev.data.l[2]    = xev.data.l[3] = xev.data.l[4] = 0;

				XSendEvent(QX11Info::display(), QX11Info::appRootWindow(), False, SubstructureNotifyMask | SubstructureRedirectMask, (XEvent*)&xev);
			}
#endif//Q_WS_X11
			QWidget::raise();
		};
	}
}


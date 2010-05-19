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

#include "classicchatwidget.h"
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
#include "ui_classicchatwidget.h"
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
namespace Core
{
	namespace AdiumChat
	{
		ClassicChatWidget::ClassicChatWidget(bool removeSessionOnClose):
				ui(new Ui::ClassicChatForm),m_remove_session_on_close(removeSessionOnClose)
		{
			m_current_index = -1;

			ui->setupUi(this);
			centerizeWidget(this);
			m_originalDoc = ui->chatEdit->document();

			//init tabbar
			ui->tabBar->setTabsClosable(true);
			ui->tabBar->setMovable(true);
			ui->tabBar->setContextMenuPolicy(Qt::CustomContextMenu);
			ui->tabButton->setIcon(Icon("view-list-text"));
			ui->contactsView->hide();
			//init status and menubar
			setAttribute(Qt::WA_DeleteOnClose);

			connect(ui->tabBar,SIGNAL(currentChanged(int)),SLOT(currentIndexChanged(int)));
			connect(ui->tabBar,SIGNAL(tabMoved(int,int)),SLOT(onTabMoved(int,int)));
			connect(ui->tabBar,SIGNAL(tabCloseRequested(int)),SLOT(onCloseRequested(int)));
			connect(ui->tabBar,SIGNAL(customContextMenuRequested(QPoint)),SLOT(onTabContextMenu(QPoint)));
			connect(ui->sendButton,SIGNAL(clicked(bool)),SLOT(onSendButtonClicked()));
			connect(ui->contactsView, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(onDoubleClicked(QModelIndex)));
			ui->contactsView->setItemDelegate(new ChatSessionItemDelegate(this));

			ui->contactsView->installEventFilter(this);
			ui->chatEdit->installEventFilter(this);
			ui->chatView->installEventFilter(this);
			ui->tabBar->installEventFilter(this);
			ui->chatEdit->setFocusPolicy(Qt::StrongFocus);

			//init toolbar
			ui->actionToolBar->setStyleSheet("QToolBar{background:none;border:none}");
			ui->actionToolBar->setIconSize(QSize(16,16));

			//for testing
			QMenu *menu = new QMenu(this);

			ui->actionToolBar->addAction(new ActionGenerator(Icon("view-history"),
															 QT_TRANSLATE_NOOP("Chat", "View History"),
															 this,
															 SLOT(onShowHistory())));

			ui->actionToolBar->addAction(new MenuActionGenerator(Icon("face-smile"),
																 QT_TRANSLATE_NOOP("Chat", "Emoticons"),
																 menu));
			QWidgetAction *emoticons_widget_act = new QWidgetAction(this);
			ChatEmoticonsWidget *emoticons_widget = new ChatEmoticonsWidget(this);
			emoticons_widget->loadTheme();
			emoticons_widget_act->setDefaultWidget(emoticons_widget);
			menu->addAction(emoticons_widget_act);
			connect(emoticons_widget,SIGNAL(insertSmile(QString)),ui->chatEdit,SLOT(appendPlainText(QString)));

			//
			//load settings
			m_html_message = Config("appearance/adiumChat").group("behavior/widget").value<bool>("htmlMessage",false);
			ConfigGroup adium_chat = Config("appearance/adiumChat").group("behavior/widget");
			m_chat_flags = static_cast<ChatFlag> (adium_chat.value<int>("widgetFlags",SendTypingNotification | ChatStateIconsOnTabs | ShowUnreadMessages | SwitchDesktopOnRaise));

			if (m_chat_flags & SendTypingNotification) {
				connect(ui->chatEdit,SIGNAL(textChanged()),SLOT(onTextChanged()));
				m_chatstate = ChatStateActive;
				m_self_chatstate_timer.setInterval(5000);
				m_self_chatstate_timer.setSingleShot(true);
				connect(&m_self_chatstate_timer,SIGNAL(timeout()),SLOT(onChatStateTimeout()));
			}

			//init shortcuts
			Shortcut *key = new Shortcut ("chatSendMessage",ui->sendButton);
			connect(key,SIGNAL(activated()),ui->sendButton,SLOT(click()));
			key = new Shortcut ("chatCloseSession",ui->tabBar);
			connect(key,SIGNAL(activated()),SLOT(closeCurrentTab()));
			key = new Shortcut ("chatNext",ui->tabBar);
			connect(key,SIGNAL(activated()),SLOT(showNextSession()));
			key = new Shortcut ("chatPrevious",ui->tabBar);
			connect(key,SIGNAL(activated()),SLOT(showPreviousSession()));
		}

		ClassicChatWidget::~ClassicChatWidget()
		{
			if (m_remove_session_on_close) {
				foreach (ChatSessionImpl *s,m_sessions) {
					s->disconnect(this);
					s->deleteLater();
				}
			}
			delete ui;
		}

		void ClassicChatWidget::addSession(ChatSessionImpl* session)
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
				icon =  ChatLayerImpl::iconForState(state);
			}

			ui->tabBar->addTab(icon,u->title());

			QAction *act = new QAction(icon,u->title(),this);
			connect(act,SIGNAL(triggered()),this,SLOT(onSessionListActionTriggered()));
			ui->tabButton->addAction(act);
		}

		void ClassicChatWidget::addSession(const ChatSessionList &sessions)
		{
			for (int i = 0; i!=sessions.count(); i++)
				addSession(sessions.at(i));
		}

		void ClassicChatWidget::currentIndexChanged(int index)
		{
			if (index == -1) {
				ui->chatEdit->setDocument(m_originalDoc);
				return;
			}
			int previous_index = m_current_index;
			m_current_index = index;
			ChatSessionImpl *session = m_sessions.at(index);

			if ((previous_index != -1) && (previous_index != index)) {
				m_sessions.at(previous_index)->setActive(false);
				session->activate();
			}
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

			ui->sendButton->setMenu(session->menu());
			ui->chatEdit->setDocument(session->getInputField());
		}

		void ClassicChatWidget::clear()
		{
			int count = m_sessions.count();
			for (int i = 0;i!=count;i++)
				ui->tabBar->removeTab(i);
			if (m_remove_session_on_close)
				qDeleteAll(m_sessions);
			m_sessions.clear();
		}

		void ClassicChatWidget::removeSession(ChatSessionImpl* session)
		{
			int index = m_sessions.indexOf(session);
			if (index == -1)
				return;
			ui->tabBar->removeTab(index);
			m_sessions.removeAt(index);
			ui->tabButton->removeAction(ui->tabButton->actions().at(index));
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

		void ClassicChatWidget::onSessionDestroyed(QObject* object)
		{
			ChatSessionImpl *sess = reinterpret_cast<ChatSessionImpl *>(object);
			removeSession(sess);
		}

		ChatSessionList ClassicChatWidget::getSessionList() const
		{
			return m_sessions;
		}

		void ClassicChatWidget::onCloseRequested(int index)
		{
			removeSession(m_sessions.at(index));
		}

		void ClassicChatWidget::onTabMoved(int from, int to)
		{
			m_sessions.move(from,to);

			//FIXME remove hack

			QList <QAction *> actions = ui->tabButton->actions();
			actions.move(from,to);
			foreach (QAction *a,ui->tabButton->actions())
				ui->tabButton->removeAction(a);
			ui->tabButton->addActions(actions);

			debug() << "moved session" << from << to;
		}

		void ClassicChatWidget::activate(AdiumChat::ChatSessionImpl* session)
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

		void ClassicChatWidget::onUnreadChanged(const qutim_sdk_0_3::MessageList &unread)
		{
			ChatSessionImpl *session = static_cast<ChatSessionImpl*>(sender());
			int index = m_sessions.indexOf(session);
			if (index < 0)
				return;
			if (unread.isEmpty()) {
				ChatState state = static_cast<ChatState>(session->property("currentChatState").toInt());
				QIcon icon =  ChatLayerImpl::iconForState(state);
				ui->tabBar->setTabIcon(index, icon);
			} else if (m_chat_flags & ShowUnreadMessages) {
				ui->tabBar->setTabIcon(index, Icon("mail-unread-new"));
			}
		}

		bool ClassicChatWidget::eventFilter(QObject *obj, QEvent *event)
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

		bool ClassicChatWidget::contains(ChatSessionImpl* session)
		{
			return m_sessions.contains(session);
		}

		void ClassicChatWidget::onSendButtonClicked()
		{
			if (ui->chatEdit->toPlainText().trimmed().isEmpty() || ui->tabBar->currentIndex() < 0)
				return;
			ChatSessionImpl *session = m_sessions.at(ui->tabBar->currentIndex());
			ChatUnit *unit = 0;
			foreach (QAction *a,ui->sendButton->menu()->actions()) {
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

		void ClassicChatWidget::chatStateChanged(ChatState state, ChatSessionImpl *session)
		{
			int index = m_sessions.indexOf(session);
			if (index == -1)
				return;

			if (m_chat_flags & ChatStateIconsOnTabs) {
				if (!session->unread().count()) {
					QIcon icon =  ChatLayerImpl::iconForState(state);
					ui->tabBar->setTabIcon(index,icon);
					ui->tabButton->actions().at(index)->setIcon(icon);
				}
			}

			session->setProperty("currentChatState",static_cast<int>(state));
		}

		QPlainTextEdit *ClassicChatWidget::getInputField()
		{
			return ui->chatEdit;
		}

		ChatSessionImpl *ClassicChatWidget::currentSession()
		{
			return m_sessions.at(m_current_index);
		}

		void ClassicChatWidget::onTextChanged()
		{
			m_self_chatstate_timer.stop();
			m_self_chatstate_timer.start();
			if ((m_chatstate != ChatStateComposing) && (!ui->chatEdit->toPlainText().isEmpty())) {
				m_chatstate = ChatStateComposing;
				m_sessions.at(ui->tabBar->currentIndex())->setChatState(m_chatstate);
			}
		}

		void ClassicChatWidget::onChatStateTimeout()
		{
			m_chatstate = ui->chatEdit->document()->isEmpty() ? ChatStateActive : ChatStatePaused;
			m_sessions.at(ui->tabBar->currentIndex())->setChatState(m_chatstate);
		}

		bool ClassicChatWidget::event(QEvent *event)
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

		void ClassicChatWidget::onTabContextMenu(const QPoint &pos)
		{
			int index = ui->tabBar->tabAt(pos);
			if (index != -1) {
				if (MenuController *session = m_sessions.value(index)->getUnit()) {
					session->showMenu(ui->tabBar->mapToGlobal(pos));
				}
			}
		}

		void ClassicChatWidget::closeCurrentTab()
		{
			if (ui->tabBar->count() > 1)
				removeSession(m_sessions.at(ui->tabBar->currentIndex()));
			else
				close();
		}


		void ClassicChatWidget::onShowHistory()
		{
			ChatUnit *unit = m_sessions.at(m_current_index)->getUnit();
			History::instance()->showHistory(unit);
		}

		void ClassicChatWidget::onSessionListActionTriggered()
		{
			QAction *act = qobject_cast<QAction *>(sender());
			Q_ASSERT(act);

			ui->tabBar->setCurrentIndex(ui->tabButton->actions().indexOf(act));
		}

		void ClassicChatWidget::showNextSession()
		{
			m_current_index++;
			if (m_current_index >= m_sessions.count())
				m_current_index = 0;
			activate(m_sessions.at(m_current_index));
		}

		void ClassicChatWidget::showPreviousSession()
		{
			m_current_index--;
			if (m_current_index < 0 )
				m_current_index = m_sessions.count() - 1;
			activate(m_sessions.at(m_current_index));
		}

		void ClassicChatWidget::onDoubleClicked(const QModelIndex &index)
		{
			Buddy *buddy = index.data(Qt::UserRole).value<Buddy*>();
			if (buddy)
				ChatLayer::get(buddy, true)->activate();
		}

		void ClassicChatWidget::onBuddiesChanged()
		{
			ChatSessionImpl *s = qobject_cast<ChatSessionImpl *>(sender());

			if (!s || (m_sessions.indexOf(s) != m_current_index))
				return;

			if (qobject_cast<Conference*>(s->getUnit()))
				ui->contactsView->setVisible(true);
			else
				ui->contactsView->setVisible(s->getModel()->rowCount(QModelIndex()) > 0);

		}

		void ClassicChatWidget::onUnitTitleChanged(const QString &title)
		{
			ChatUnit *u = qobject_cast<ChatUnit *>(sender());
			if (!u)
				return;
			ChatSessionImpl *s = static_cast<ChatSessionImpl *>(ChatLayerImpl::get(u,false));
			if (!s)
				return;
			ui->tabBar->setTabText(m_sessions.indexOf(s),title);
		}

		void ClassicChatWidget::raise()
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

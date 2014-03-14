/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "tabbedchatwidget.h"
#include <qutim/servicemanager.h>
#include <qutim/adiumchat/chatviewfactory.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qutim/actiontoolbar.h>
#include "tabbar.h"
#include <qutim/adiumchat/chatedit.h>
#include <qutim/adiumchat/conferencecontactsview.h>
#include <QTextEdit>
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <qutim/conference.h>
#include <qutim/config.h>
#include <qutim/qtwin.h>
#include <QAbstractItemModel>
#include <QSplitter>
#include <qutim/shortcut.h>
#include <QToolButton>
#include <QMenuBar>
#include <qutim/account.h>
#include <QSwipeGesture>
#include <QDockWidget>

namespace Core
{
namespace AdiumChat
{

TabbedChatWidget::TabbedChatWidget(const QString &key, QWidget *parent) :
	AbstractChatWidget(parent),
    m_toolbar(new ActionToolBar(tr("Chat Actions"),this)),
	m_actions(ActionContainer::TypeMatch, ActionTypeChatButton),
	m_chatViewWidget(0),
	m_tabBar(new TabBar(this)),
	m_chatInput(new ChatEdit(this)),
	m_receiverList(new QAction(Icon("view-choose"),tr("Send to"),this)),
	m_contactView(new ConferenceContactsView(this)),
	m_key(key),
	m_layout(0),
	m_chatViewLayout(0),
	m_unitAction(0),
	m_vSplitter(new QSplitter(Qt::Vertical, this)),
	m_view(0),
	m_hSplitter(0)
{
    m_actions.addHandler(this);
    m_actions.show();
    setAttribute(Qt::WA_DeleteOnClose);
	setCentralWidget(new QWidget(this));
	ServicePointer<ChatViewFactory> factory("ChatViewFactory");
    setView(factory->createViewWidget());

	m_vSplitter->setObjectName(QLatin1String("vSplitter"));
	m_vSplitter->addWidget(m_chatInput);

	m_hSplitter = new QSplitter(Qt::Horizontal, this);
	m_hSplitter->setObjectName(QLatin1String("hSplitter"));
	m_hSplitter->addWidget(m_vSplitter);
	m_hSplitter->addWidget(m_contactView);

	m_layout = new QVBoxLayout(centralWidget());
	m_layout->addWidget(m_hSplitter);
#ifdef Q_OS_MAC
	m_layout->setMargin(0);
	m_layout->setSpacing(1);
#endif

	m_actSeparator = m_toolbar->addSeparator();
	m_unitSeparator = m_toolbar->addSeparator();
	m_sessionList = new QAction(Icon("view-list-tree"),tr("Session list"),this);
	m_sessionList->setMenu(m_tabBar->menu());
	Shortcut *shortcut = new Shortcut(QLatin1String("chatListSession"), this);
	connect(shortcut, SIGNAL(activated()), m_sessionList, SLOT(trigger()));
	m_tabBar->setObjectName(QLatin1String("chatBar"));

	loadSettings();

	connect(m_tabBar, SIGNAL(remove(ChatSessionImpl*)), SLOT(removeSession(ChatSessionImpl*)));
	ensureToolBar();
}

void TabbedChatWidget::setView(QWidget *view)
{
	if (m_view)
		m_view->deleteLater();

	if (!m_chatViewLayout) {
		QWidget *w = new QWidget(this);
		m_chatViewLayout = new QVBoxLayout(w);
		m_chatViewLayout->setMargin(0);
		m_vSplitter->insertWidget(0, w);
	}
	m_chatViewLayout->addWidget(view);
	m_chatViewWidget = qobject_cast<ChatViewWidget*>(view);

	if (QAbstractScrollArea *area = qobject_cast<QAbstractScrollArea*>(view)) {
		area->viewport()->grabGesture(Qt::SwipeGesture);
		area->viewport()->installEventFilter(this);
	} else {
		view->grabGesture(Qt::SwipeGesture);
		view->installEventFilter(this);
	}
	m_view = view;
}

void TabbedChatWidget::loadSettings()
{
	ConfigGroup cfg = Config("appearance").group("chat/behavior/widget");
	if(!property("loaded").toBool()) {
		m_flags =  cfg.value("widgetFlags", IconsOnTabs
							 | DeleteSessionOnClose
							 | SwitchDesktopOnActivate
							 | AdiumToolbar
							 | TabsOnBottom
							 );

		//simple options check for roster
		if (m_flags & ShowRoster) {
			m_flags &= ~AdiumToolbar;
			m_flags &= ~TabsOnBottom;
		}

		QWidget *tabBar = m_tabBar;
		if( m_flags & AdiumToolbar) {
			addToolBar(Qt::TopToolBarArea, m_toolbar);

			//simple hack
			m_receiverList->setMenu(new QMenu);
			m_toolbar->addAction(m_receiverList);
			m_receiverList->menu()->deleteLater();

			QWidget* spacer = new QWidget(this);
			spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			spacer->setAttribute(Qt::WA_TransparentForMouseEvents);
			m_toolbar->addWidget(spacer);
			m_toolbar->addAction(m_sessionList);
			m_toolbar->setIconSize(QSize(22, 22));
			setUnifiedTitleAndToolBar(true);
		} else {
			if (m_flags & ShowRoster)
				m_chatViewLayout->addWidget(m_toolbar);
			else
				m_layout->addWidget(m_toolbar);
			m_toolbar->setIconSize(QSize(16,16));
			m_toolbar->setStyleSheet(QLatin1String("QToolBar{background:none;border:none;}"));
			QWidget* spacer = new QWidget(this);
			spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			spacer->setAttribute(Qt::WA_TransparentForMouseEvents);
			m_toolbar->addWidget(spacer);

			QToolButton *btn = new QToolButton(this);
			m_receiverList->setText(tr("Send"));
			btn->setToolButtonStyle(Qt::ToolButtonTextOnly);
			//simple hack
			m_receiverList->setMenu(new QMenu);
			btn->setDefaultAction(m_receiverList);
			m_receiverList->menu()->deleteLater();
			connect(btn,SIGNAL(clicked(bool)),m_chatInput,SLOT(send()));
			m_toolbar->addWidget(btn);
			btn->setAutoRaise(false);

			tabBar = new QWidget(this);
			tabBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
			QHBoxLayout *l = new QHBoxLayout(tabBar);
			l->setMargin(0);
			btn = new QToolButton(this);
			btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
			btn->setAutoRaise(true);
			btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			btn->setPopupMode(QToolButton::InstantPopup);
			btn->setDefaultAction(m_sessionList);
			l->addWidget(m_tabBar);
			l->addWidget(btn);
		}

		if(m_flags & TabsOnBottom) {
			m_layout->addWidget(tabBar);
			m_tabBar->setShape(QTabBar::RoundedSouth);
			setContentsMargins(1, 1, 1, 1);
#ifdef Q_OS_MAC
			m_tabBar->setDocumentMode(false);
			m_tabBar->setDrawBase(false);
#endif
		} else {
			m_layout->insertWidget(0, tabBar);
#ifdef Q_OS_MAC
			m_tabBar->setDocumentMode(true);
			m_tabBar->setDrawBase(true);
			setContentsMargins(0, 0, 0, 1);
#endif
		}

		if (m_flags & UseQutimIcon)
			m_attributes &= ~UseCustomIcon;
		else
			m_attributes |= UseCustomIcon;

		ServicePointer<MenuController> contactList("ContactList");
		if (m_flags & MenuBar) {
			setMenuBar(new QMenuBar(this));

			if (contactList) {
				QAction *general = menuBar()->addAction(tr("&Actions"));
				general->setMenu(contactList->menu(false));
			}

			QAction *accounts = menuBar()->addAction(tr("Accoun&ts"));
			QMenu *menu = new QMenu(this);
            foreach(Account *account, Account::all()) {
				QMenu *accountMenu = account->menu(false);
				menu->addMenu(accountMenu);
			}
			accounts->setMenu(menu);
			m_unitAction = menuBar()->addAction(tr("&Chat"));
		}
		if (m_flags & ShowRoster && !contactList.isNull()) {
			QWidget *roster = 0;
			contactList->metaObject()->invokeMethod(contactList.data(), "widget",
													Q_RETURN_ARG(QWidget*, roster));

			if (roster) {
				if (!m_dockWidget) {
					m_dockWidget = new QDockWidget(tr("Contacts"), this);
					m_dockWidget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
					addDockWidget(cfg.value("RosterPosition", Qt::LeftDockWidgetArea), m_dockWidget.data());
				}
				//m_hSplitter->insertWidget(0, roster);
				m_roster = roster;
				m_dockWidget->setWidget(roster);
			}
		} else {
			if (m_roster) {
				//TODO remove splitter
				m_roster->setParent(0);
				m_dockWidget->deleteLater();
			}
		}

		ConfigGroup keyGroup = cfg.group("keys");
		qDebug() << "Load new settings" << keyGroup.childGroups();
		if (keyGroup.hasChildGroup(m_key)) {
			qDebug() << "load settings for key" << m_key;
			keyGroup.beginGroup(m_key);
			QByteArray geom = keyGroup.value("geometry", QByteArray());
			restoreGeometry(geom);
			foreach (QSplitter *splitter, findChildren<QSplitter*>()) {
				geom = keyGroup.value(splitter->objectName(), QByteArray());
				qDebug() << "found splitter" << geom;
				splitter->restoreState(geom);
			}
			keyGroup.endGroup();
		} else {
			resize(600, 400);
			centerizeWidget(this);
		}
		setProperty("loaded",true);
	}
	m_chatInput->setSendKey(cfg.value("sendKey", SendCtrlEnter));
	m_chatInput->setAutoResize(cfg.value("autoResize", false));

	if(m_flags & IconsOnTabs)
		m_tabBar->setIconSize(QSize(16,16)); //TODO
	else
		m_tabBar->setIconSize(QSize(0,0));
}

TabbedChatWidget::~TabbedChatWidget()
{
	ConfigGroup group = Config("appearance").group("chat/behavior/widget");
	Qt::DockWidgetArea area = dockWidgetArea(m_dockWidget);
	if (area != Qt::NoDockWidgetArea && m_dockWidget->isAreaAllowed(area))
		group.setValue("RosterPosition", area);
	group.beginGroup("keys");
	group.beginGroup(m_key);
	group.setValue("geometry", saveGeometry());
	foreach (QSplitter *splitter, findChildren<QSplitter*>()) {
		group.setValue(splitter->objectName(), splitter->saveState());
	}

	group.sync();

	delete m_tabBar;
	delete m_chatInput;
	delete m_contactView;
	if (m_roster)
		m_roster->setParent(0);
}

QTextEdit *TabbedChatWidget::getInputField() const
{
	return m_chatInput;
}

bool TabbedChatWidget::contains(ChatSessionImpl *session) const
{
	return m_tabBar->contains(session);
}

void TabbedChatWidget::actionAdded(QAction *action, int index)
{
	QList<QAction*> actions = m_toolbar->actions();
	int unitIndex = actions.indexOf(m_unitSeparator);
	index = unitIndex - index;
	m_toolbar->insertAction(actions.at(index), action);
}

void TabbedChatWidget::actionRemoved(int index)
{
	QList<QAction*> actions = m_toolbar->actions();
	int unitIndex = actions.indexOf(m_unitSeparator);
	index = unitIndex - index;
	m_toolbar->removeAction(actions.at(index));
}

void TabbedChatWidget::actionsCleared()
{
	QList<QAction*> actions = m_toolbar->actions();
	int from = actions.indexOf(m_actSeparator) + 1;
	int to = actions.indexOf(m_unitSeparator, from);
	for (int i = from; i < to; i++)
		m_tabBar->removeAction(actions.at(i));
}

void TabbedChatWidget::addAction(ActionGenerator *gen)
{
	m_toolbar->insertAction(m_actSeparator,gen);
}

void TabbedChatWidget::addSession(ChatSessionImpl *session)
{
	if(!session->isActive())
		return;
	m_tabBar->addSession(session);

	connect(session, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
	connect(session, SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)), SLOT(onUnreadChanged()));
	connect(session, SIGNAL(controllerDestroyed(QObject*)),
			this, SLOT(onControllerDestroyed(QObject*)));
}

void TabbedChatWidget::removeSession(ChatSessionImpl *session)
{
	if(contains(session))
		m_tabBar->removeSession(session);
	if(m_flags & DeleteSessionOnClose) {
		session->deleteLater();
	} else
		session->setActive(false);

	if(!m_tabBar->count())
		deleteLater();
}

void TabbedChatWidget::onSessionActivated(bool active)
{
	if(!active)
		return;

	ChatSessionImpl *session = qobject_cast<ChatSessionImpl*>(sender());
	Q_ASSERT(session);
	m_tabBar->setCurrentSession(session);
}

void TabbedChatWidget::activate(ChatSessionImpl *session)
{
	if(!session->unread().isEmpty())
		session->markRead();

	activateWindow();
	setTitle(session);

	if(m_currentSession) {
		if(m_currentSession.data() == session)
			return;
		m_currentSession.data()->setActive(false);
	}
	emit currentSessionChanged(session, m_currentSession.data());
	m_currentSession = session;

	m_tabBar->setCurrentSession(session);
	m_chatInput->setSession(session);
	m_contactView->setSession(session);
	m_chatViewWidget->setViewController(session->controller());

	m_actions.setController(session->getUnit());
	m_receiverList->setMenu(session->menu(m_flags & ShowReceiverId));

	if(m_flags & MenuBar) {
		delete m_unitAction->menu();
		QMenu *menu = session->unit()->menu(false);
		connect(m_unitAction, SIGNAL(destroyed()), menu, SLOT(deleteLater()));
		m_unitAction->setMenu(menu);
	}
}

ChatSessionImpl *TabbedChatWidget::currentSession() const
{
	return m_tabBar->currentSession();
}

bool TabbedChatWidget::event(QEvent *event)
{
	if (event->type() == QEvent::WindowActivate
			|| event->type() == QEvent::WindowDeactivate) {
		bool active = event->type() == QEvent::WindowActivate;
		if (!m_tabBar->currentSession())
			return false;
		m_tabBar->currentSession()->setActive(active);
	} else if (event->type() == QEvent::Show)
		if (unifiedTitleAndToolBarOnMac())
			ensureToolBar();
	return AbstractChatWidget::event(event);
}

bool TabbedChatWidget::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::Gesture) {
		QGestureEvent *ge = static_cast<QGestureEvent*>(event);
		if (QGesture *gesture = ge->gesture(Qt::SwipeGesture)) {
			QSwipeGesture *swipe = static_cast<QSwipeGesture*>(gesture);
			if (swipe->state() == Qt::GestureFinished) {
				if (swipe->horizontalDirection() == QSwipeGesture::Right)
					m_tabBar->showNextTab();
				else
					m_tabBar->showPreviousTab();
				ge->accept(gesture);
			}
		}
	}
	return QMainWindow::eventFilter(obj, event);
}

void TabbedChatWidget::setUnifiedTitleAndToolBar(bool set)
{
    setUnifiedTitleAndToolBarOnMac(set);
    if (!set)
		return;
	m_toolbar->setMovable(false);
	m_toolbar->setMoveHookEnabled(true);

	connect(m_toolbar, SIGNAL(iconSizeChanged(QSize)), SLOT(ensureToolBar()));
	connect(m_toolbar, SIGNAL(toolButtonStyleChanged(Qt::ToolButtonStyle)), SLOT(ensureToolBar()));
}

void TabbedChatWidget::ensureToolBar()
{
    if (QtWin::isCompositionEnabled() && (m_flags & AdiumToolbar)) {
        m_toolbar->setStyleSheet("QToolBar{background:none;border:none;}");
        centralWidget()->setAutoFillBackground(true);
		QtWin::extendFrameIntoClientArea(this,
										 0,
										 0,
										 m_toolbar->sizeHint().height(),
										 0
										 );
		setContentsMargins(0, 0, 0, 0);
    }
}

void TabbedChatWidget::activateWindow()
{
	AbstractChatWidget::activateWindow();
}

void TabbedChatWidget::onUnreadChanged()
{
	ChatSessionImpl *s = qobject_cast<ChatSessionImpl*>(sender());
	if(s && s == m_tabBar->currentSession())
		setTitle(s);
}

void TabbedChatWidget::onControllerDestroyed(QObject *)
{
	if (m_tabBar->currentSession() == sender())
		m_chatViewWidget->setViewController(m_tabBar->currentSession()->controller());
}

ActionToolBar *TabbedChatWidget::toolBar() const
{
	return m_toolbar;
}

}
}


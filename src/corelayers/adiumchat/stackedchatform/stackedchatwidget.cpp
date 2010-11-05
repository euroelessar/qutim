#include "stackedchatwidget.h"
#include <qutim/servicemanager.h>
#include <chatlayer/chatviewfactory.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qutim/actiontoolbar.h>
#include "sessionlistwidget.h"
#include <chatlayer/chatedit.h>
#include <chatlayer/conferencecontactsview.h>
#include <QPlainTextEdit>
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <qutim/conference.h>
#include <qutim/config.h>
#include <qutim/qtwin.h>
#include <QAbstractItemModel>
#include <QSplitter>
#include <qutim/shortcut.h>
#include <QToolButton>
#include <slidingstackedwidget.h>

#ifdef Q_WS_X11
# include <QX11Info>
# include <X11/Xutil.h>
# include <X11/Xlib.h>
# include <X11/Xatom.h>
# ifdef KeyPress
#  undef KeyPress
# endif
# define MESSAGE_SOURCE_OLD            0
# define MESSAGE_SOURCE_APPLICATION    1
# define MESSAGE_SOURCE_PAGER          2
#ifdef Status
# undef Status
#endif
#endif //Q_WS_X11

namespace Core
{
namespace AdiumChat
{

StackedChatWidget::StackedChatWidget(const QString &key, QWidget *parent) :
	AbstractChatWidget(parent),
	m_toolbar(new ActionToolBar(tr("Chat Actions"),this)),
	m_sessionList(new SessionListWidget(this)),
	m_chatInput(new ChatEdit(this)),
	m_recieverList(new QAction(Icon("view-choose"),tr("Send to"),this)),
	m_contactView(new ConferenceContactsView(this)),
	m_key(key)
{
	setAttribute(Qt::WA_DeleteOnClose);

	m_stack = new SlidingStackedWidget(this);

	m_chatWidget = new QWidget(m_stack);
	setCentralWidget(m_stack);
	QWidget *view = ChatViewFactory::instance()->createViewWidget();
	view->setParent(m_chatWidget);

	m_stack->addWidget(m_sessionList);
	m_stack->addWidget(m_chatWidget);
	m_stack->setWrap(true);

	QSplitter *vSplitter = new QSplitter(Qt::Vertical,this);
	vSplitter->setObjectName(QLatin1String("vSplitter"));
	vSplitter->addWidget(view);
	vSplitter->addWidget(m_chatInput);

	QVBoxLayout *layout = new QVBoxLayout(m_chatWidget);
	layout->addWidget(vSplitter);
	layout->setMargin(0);

	m_view = qobject_cast<ChatViewWidget*>(view);

	m_actSeparator = m_toolbar->addSeparator();
	m_unitSeparator = m_toolbar->addSeparator();

	addToolBar(Qt::RightToolBarArea,m_toolbar);

	//simple hack
	m_recieverList->setMenu(new QMenu);
	m_toolbar->addAction(m_recieverList);
	m_recieverList->menu()->deleteLater();

	QWidget* spacer = new QWidget(this);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	spacer->setAttribute(Qt::WA_TransparentForMouseEvents);
	m_toolbar->addWidget(spacer);

	loadSettings();

	connect(m_sessionList,SIGNAL(remove(ChatSessionImpl*)),SLOT(removeSession(ChatSessionImpl*)));

	//only for testing
	QAction *act = new QAction(Icon("arrow-left"),tr("left"),this);
	connect(act,SIGNAL(triggered()),m_stack,SLOT(slideInPrev()));
	m_toolbar->addAction(act);
	act = new QAction(Icon("arrow-right"),tr("right"),this);
	connect(act,SIGNAL(triggered()),m_stack,SLOT(slideInNext()));
	m_toolbar->addAction(act);
}

void StackedChatWidget::loadSettings()
{
	ConfigGroup cfg = Config("appearance").group("chat/behavior/widget");
	if(!property("loaded").toBool()) {
		m_flags =  cfg.value("widgetFlags", SendTypingNotification
							 | DeleteSessionOnClose
							 | SwitchDesktopOnActivate
							 );

		ConfigGroup keyGroup = cfg.group("keys");
		qDebug() << "Load new settings" << keyGroup.childGroups();
		if (keyGroup.hasChildGroup(m_key)) {
			debug() << "load settings for key" << m_key;
			keyGroup.beginGroup(m_key);
			QByteArray geom = keyGroup.value("geometry", QByteArray());
			restoreGeometry(geom);
			foreach (QSplitter *splitter, findChildren<QSplitter*>()) {
				geom = keyGroup.value(splitter->objectName(), QByteArray());
				debug() << "found splitter" << geom;
				splitter->restoreState(geom);
			}
			keyGroup.endGroup();
		} else {
			resize(600,400);
			centerizeWidget(this);
		}
		setProperty("loaded",true);
	}
	m_chatInput->setSendKey(cfg.value("sendKey", SendCtrlEnter));
}

StackedChatWidget::~StackedChatWidget()
{
	delete m_sessionList;
	ConfigGroup group = Config("appearance").group("chat/behavior/widget/keys").group(m_key);
	group.setValue("geometry", saveGeometry());
	foreach (QSplitter *splitter, findChildren<QSplitter*>()) {
		group.setValue(splitter->objectName(), splitter->saveState());
	}
	group.sync();
}

QPlainTextEdit *StackedChatWidget::getInputField() const
{
	return m_chatInput;
}

bool StackedChatWidget::contains(ChatSessionImpl *session) const
{
	return m_sessionList->contains(session);
}

void StackedChatWidget::addAction(ActionGenerator *gen)
{
	m_toolbar->insertAction(m_actSeparator,gen);
}

void StackedChatWidget::addSession(ChatSessionImpl *session)
{
	m_sessionList->addSession(session);
	connect(session,SIGNAL(activated(bool)),SLOT(onSessionActivated(bool)));
	connect(session,SIGNAL(unreadChanged(qutim_sdk_0_3::MessageList)),SLOT(onUnreadChanged()));
}

void StackedChatWidget::removeSession(ChatSessionImpl *session)
{
	if(contains(session))
		m_sessionList->removeSession(session);
	session->setActive(false);
	//TODO delete on close flag
	session->deleteLater();

	if(!m_sessionList->count())
		deleteLater();
}

void StackedChatWidget::onSessionActivated(bool active)
{
	if(!active)
		return;

	ChatSessionImpl *session = qobject_cast<ChatSessionImpl*>(sender());
	Q_ASSERT(session);

	m_sessionList->setCurrentSession(session);
}

void StackedChatWidget::activate(ChatSessionImpl *session)
{
	if(!session->unread().isEmpty())
		session->markRead();

	activateWindow();
	setTitle(session);

	if(m_currentSession) {
		if(m_currentSession == session)
			return;
		m_currentSession->setActive(false);
	}
	emit currentSessionChanged(session,m_currentSession);
	m_currentSession = session;

	m_view->setViewController(session->getController());
	m_sessionList->setCurrentSession(session);
	m_chatInput->setSession(session);
	m_contactView->setSession(session);

	if(m_contactView->isVisible())
		m_stack->addWidget(m_contactView);
	else
		m_stack->removeWidget(m_contactView);
	m_stack->slideInIdx(m_stack->indexOf(m_chatWidget));

	qDeleteAll(m_unitActions);
	m_unitActions.clear();
	m_recieverList->setMenu(session->menu());
	ActionContainer container(session->getUnit(),ActionContainer::TypeMatch,ActionTypeChatButton);
	for (int i = 0;i!=container.count();i++) {
		QAction *current = container.action(i);
		m_toolbar->insertAction(m_unitSeparator,current);
		m_unitActions.append(current);
	}
}

ChatSessionImpl *StackedChatWidget::currentSession() const
{
	return m_sessionList->currentSession();
}

bool StackedChatWidget::event(QEvent *event)
{
	if (event->type() == QEvent::WindowActivate
			|| event->type() == QEvent::WindowDeactivate) {
		bool active = event->type() == QEvent::WindowActivate;
		if (!m_sessionList->currentSession())
			return false;
		m_sessionList->currentSession()->setActive(active);
	}
	return AbstractChatWidget::event(event);
}

void StackedChatWidget::activateWindow()
{
#ifdef Q_WS_X11
	if (m_flags & SwitchDesktopOnActivate) {
		static Atom NET_ACTIVE_WINDOW = 0;
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
	AbstractChatWidget::activateWindow();
}

void StackedChatWidget::onUnreadChanged()
{
	ChatSessionImpl *s = qobject_cast<ChatSessionImpl*>(sender());
	if(s && s == m_sessionList->currentSession())
		setTitle(s);
}

ActionToolBar *StackedChatWidget::toolBar() const
{
	return m_toolbar;
}

}
}


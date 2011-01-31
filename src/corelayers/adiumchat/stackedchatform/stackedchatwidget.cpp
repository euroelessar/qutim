#include "stackedchatwidget.h"
#include <qutim/servicemanager.h>
#include <chatlayer/chatviewfactory.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qutim/actiontoolbar.h>
#include "sessionlistwidget.h"
#include "fingerswipegesture.h"
#include "floatingbutton.h"
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
#include <qutim/servicemanager.h>
#include <QApplication>
#include <QDesktopWidget>
#ifdef Q_WS_MAEMO_5
#include <QAbstractKineticScroller>
#include <kb_qwerty.h>
#endif

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
	m_recieverList(new QAction(tr("Send to"),this)),
	m_unitActions(new QAction(Icon("preferences-contact-list"),tr("Actions"),this)),
	m_contactView(new ConferenceContactsView(this)),
	m_key(key)
{
	m_stack = new SlidingStackedWidget(this);

	m_chatWidget = new QWidget(m_stack);
	setCentralWidget(m_stack);

	QWidget *view = ChatViewFactory::instance()->createViewWidget();

	QScrollArea *chatViewScrollArea = new QScrollArea();

	QWidget *chatViewWidget = new QWidget;

	view->setParent(chatViewWidget);

	QVBoxLayout *chatViewLayout = new QVBoxLayout(chatViewWidget);
	chatViewLayout->addWidget(view);
	chatViewLayout->setMargin(0);

	chatViewScrollArea->setWidget(chatViewWidget);
	chatViewScrollArea->setWidgetResizable(true);

	m_stack->addWidget(m_sessionList);
	m_stack->addWidget(m_chatWidget);
	m_stack->setWrap(true);

	QWidget *chatInputWidget = new QWidget(m_chatWidget);
	QHBoxLayout *chatInputLayout = new QHBoxLayout(chatInputWidget);
	chatInputLayout->setMargin(0);
	chatInputLayout->addWidget(m_toolbar);
	chatInputLayout->addWidget(m_chatInput);
	QToolBar *sendToolBar = new QToolBar(m_chatWidget);
#ifndef QUTIM_MOBILE_UI
	sendToolBar->addAction(m_unitActions);
	sendToolBar->setOrientation(Qt::Vertical);
#endif
	sendToolBar->addAction(m_recieverList);
	chatInputLayout->addWidget(sendToolBar);

	QSplitter *vSplitter = new QSplitter(Qt::Vertical,this);
	vSplitter->setObjectName(QLatin1String("vSplitter"));
	vSplitter->addWidget(chatViewScrollArea);
	vSplitter->addWidget(chatInputWidget);

	QVBoxLayout *layout = new QVBoxLayout(m_chatWidget);
	layout->addWidget(vSplitter);
	layout->setMargin(0);
#ifdef Q_WS_MAEMO_5
	m_kb_qwerty = new kb_Qwerty(this);
	layout->addWidget(m_kb_qwerty);
#endif

	m_view = qobject_cast<ChatViewWidget*>(view);

	menuBar = new QMenuBar(m_stack);
	m_toolbar->setOrientation(Qt::Vertical);

	loadSettings();

	connect(m_sessionList,SIGNAL(remove(ChatSessionImpl*)),SLOT(removeSession(ChatSessionImpl*)));
	connect(m_stack,SIGNAL(currentChanged(int)),SLOT(onCurrentChanged(int)));


#ifndef Q_WS_MAEMO_5
	m_recieverList->setIcon(Icon("view-choose"));
#else
	m_recieverList->setIcon(Icon("chat_enter"));
#endif
	connect(m_recieverList,SIGNAL(triggered()),m_chatInput,SLOT(send()));

	setAttribute(Qt::WA_AcceptTouchEvents);
	//fingerSwipeGestureType = (Qt::GestureType)0;
	//fingerSwipeGestureType = QGestureRecognizer::registerRecognizer( new FingerSwipeGestureRecognizer() );
	//grabGesture(fingerSwipeGestureType);
	connect(m_stack,SIGNAL(animationFinished()),this,SLOT(animationFinished()));
	connect(m_stack,SIGNAL(fingerGesture(enum SlidingStackedWidget::SlideDirection)),this,SLOT(fingerGesture(enum SlidingStackedWidget::SlideDirection)));

	FloatingButton *chatNext=new FloatingButton(3,m_chatWidget);
	FloatingButton *sessionListNext=new FloatingButton(0,m_sessionList);
	FloatingButton *contactViewNext=new FloatingButton(0,m_contactView);
	connect(chatNext,SIGNAL(clicked()),m_stack,SLOT(slideInNext()));
	connect(sessionListNext,SIGNAL(clicked()),m_stack,SLOT(slideInNext()));
	connect(contactViewNext,SIGNAL(clicked()),m_stack,SLOT(slideInNext()));

	FloatingButton *chatClose=new FloatingButton(1,m_chatWidget);
	connect(chatClose,SIGNAL(clicked()),m_sessionList,SLOT(closeCurrentSession()));

	FloatingButton *showContactList=new FloatingButton(2,m_chatWidget);
	connect(showContactList,SIGNAL(clicked()),this,SLOT(showContactList()));

#if defined(Q_WS_MAEMO_5)
	connect(m_kb_qwerty,SIGNAL(input(QString)),this,SLOT(processInput(QString)));

//    	QAbstractKineticScroller *scroller = m_chatInput->property("kineticScroller") .value<QAbstractKineticScroller *>();
//    	if (scroller)
//    	{
//		scroller->setEnabled(true);
//		scroller->setOvershootPolicy(QAbstractKineticScroller::OvershootAlwaysOff);
//	}
#endif
	connect(qApp->desktop(), SIGNAL(resized(int)), this, SLOT(orientationChanged()));
	orientationChanged();

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
	m_chatInput->setSendKey(cfg.value("sendKey", SendEnter));
}

StackedChatWidget::~StackedChatWidget()
{
	if (QObject *obj = ServiceManager::getByName("ContactList"))
		obj->metaObject()->invokeMethod(obj, "show");
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

	if(session == m_sessionList->currentSession()) {
		m_view->setViewController(0);
		m_chatInput->setDocument(0);
	}

	session->setActive(false);
	if(m_flags & DeleteSessionOnClose)
		session->deleteLater();

	m_stack->slideInIdx(m_stack->indexOf(m_sessionList));

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

	bool isActivateWindow = false;
	if (qApp->activeWindow() != this)
	{
		activateWindow();
		isActivateWindow = true;
	}

	setTitle(session);

	if(m_currentSession) {
		if(m_currentSession == session) {
			m_stack->slideInIdx(m_stack->indexOf(m_chatWidget));
			return;
		}
		m_currentSession->setActive(false);
	}
	emit currentSessionChanged(session,m_currentSession);
	m_currentSession = session;

	m_sessionList->setCurrentSession(session);
	m_chatInput->setSession(session);
	m_contactView->setSession(session);
	m_view->setViewController(session->getController());

	if(m_contactView->isVisible())
		m_stack->addWidget(m_contactView);
	else
		m_stack->removeWidget(m_contactView);

	if (!isActivateWindow)
		m_stack->slideInIdx(m_stack->indexOf(m_chatWidget));
	else
		m_stack->setCurrentIndex(m_stack->indexOf(m_chatWidget));

	menuBar->clear();
	menuBar->addMenu(session->getUnit()->menu());
	m_recieverList->setMenu(session->menu());
#ifndef QUTIM_MOBILE_UI
	m_unitActions->setMenu(session->getUnit()->menu());
#endif

}

ChatSessionImpl *StackedChatWidget::currentSession() const
{
	return m_sessionList->currentSession();
}

bool StackedChatWidget::event(QEvent *event)
{
//	if (event->type() == QEvent::TouchBegin) {

//		event->accept();
//		return true;
//	}

//	if (event->type() == QEvent::Gesture) {
//		QGestureEvent *ge = static_cast<QGestureEvent*>(event);

//		if (QGesture *gesture = ge->gesture(fingerSwipeGestureType)) {
//		    FingerSwipeGesture *swipe = static_cast<FingerSwipeGesture*>(gesture);
//		    if (swipe->state() == Qt::GestureFinished) {
//			    if (swipe->isLeftToRight())
//			    {
//				    m_stack->slideInPrev();
//				    m_contactView->blockSignals(true);
//			    }
//			    else if (swipe->isRightToLeft())
//			    {
//				    m_stack->slideInNext();
//				    m_contactView->blockSignals(true);
//			    }

//			}

//			ge->setAccepted(gesture, true);
//			return true;
//		    }
//	}

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

void StackedChatWidget::onCurrentChanged(int index)
{
	if(index != m_stack->indexOf(m_chatWidget))
		currentSession()->setActive(false);
	m_toolbar->setVisible(index == m_stack->indexOf(m_chatWidget));
}

void StackedChatWidget::fingerGesture( enum SlidingStackedWidget::SlideDirection direction)
{
    if (direction==SlidingStackedWidget::LeftToRight)
    {
	m_stack->slideInPrev();
	m_contactView->blockSignals(true);
    }
    else if (direction==SlidingStackedWidget::RightToLeft)
    {
	m_stack->slideInNext();
	m_contactView->blockSignals(true);
    }

}

void StackedChatWidget::animationFinished()
{
	m_contactView->blockSignals(false);
}

void StackedChatWidget::orientationChanged()
{
#ifdef Q_WS_MAEMO_5
	QRect screenGeometry = QApplication::desktop()->screenGeometry();
	if (screenGeometry.width() > screenGeometry.height())
	{
		qApp->setAutoSipEnabled(true);
		m_kb_qwerty->setVisible(false);
	}
	else
	{
		qApp->setAutoSipEnabled(false);
		m_kb_qwerty->setVisible(true);

	}
#endif
}

void StackedChatWidget::showContactList()
{
	if (QObject *obj = ServiceManager::getByName("ContactList"))
		obj->metaObject()->invokeMethod(obj, "show");
}

void StackedChatWidget::processInput(QString sInput)
{
#ifdef Q_WS_MAEMO_5
	m_chatInput->setFocus();

	if(sInput.compare(kb_Qwerty::DELETE) == 0)
	{
		m_chatInput->textCursor().deletePreviousChar();
	}
	else
	{
		m_chatInput->textCursor().insertText(sInput);
	}

	//Scroll text edit if necessary
	m_chatInput->ensureCursorVisible();
#endif
}

}

}


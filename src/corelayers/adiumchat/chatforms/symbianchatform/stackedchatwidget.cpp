#include "stackedchatwidget.h"
#include <qutim/servicemanager.h>
#include <chatlayer/chatviewfactory.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <qutim/actiontoolbar.h>
#include <chatlayer/sessionlistwidget.h>
#include "fingerswipegesture.h"
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
#include <qutim/actionbox.h>
#include <qutim/servicemanager.h>

namespace Core
{
namespace AdiumChat
{
namespace Symbian
{

StackedChatWidget::StackedChatWidget(QWidget *parent) :
	AbstractChatWidget(parent),
	m_chatInput(new ChatEdit(this)),
	m_recieverList(new QAction(tr("Send to"),this)),
	m_stack(new SlidingStackedWidget(this)),
	m_chatWidget(new QWidget(m_stack)),
	m_unitActions(new QAction(tr("Actions"),m_chatWidget)),
	m_menu(new MenuController(this))
{
	QWidget *widget = new QWidget(this);
	QVBoxLayout *l = new QVBoxLayout(widget);
	l->setMargin(0);
	l->addWidget(m_stack);

	setCentralWidget(widget);
	QWidget *view = ChatViewFactory::instance()->createViewWidget();
	view->setParent(m_chatWidget);

	//TODO move to chatform
	//FIXME Create session list and chat when it's realy needed
	if (QObject *obj = ServiceManager::getByName("ContactList")) {
		QWidget *w;
		obj->metaObject()->invokeMethod(obj, "widget",Q_RETURN_ARG(QWidget*, w));
		m_stack->addWidget(w);
		m_stack->setCurrentWidget(w);
	}
	m_stack->setWrap(true);
	m_sessionList = new SessionListWidget(this);
	m_contactView = new ConferenceContactsView(this);

	QWidget *chatInputWidget = new QWidget(m_chatWidget);
	chatInputWidget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
	chatInputWidget->setMaximumHeight(m_chatInput->minimumHeight());

	QHBoxLayout *chatInputLayout = new QHBoxLayout(chatInputWidget);
	chatInputLayout->setMargin(0);
	chatInputLayout->setSpacing(0);

	QToolButton *leftBtn = new QToolButton(this);
	leftBtn->setIcon(Icon("preferences-contact-list"));
	leftBtn->setMenu(m_menu->menu());
	leftBtn->setPopupMode(QToolButton::InstantPopup);

	QToolButton *rightBtn = new QToolButton(this);
	rightBtn->setDefaultAction(m_recieverList);

	chatInputLayout->setMargin(0);
	chatInputLayout->addWidget(leftBtn);
	chatInputLayout->addWidget(m_chatInput);
	chatInputLayout->addWidget(rightBtn);

	QVBoxLayout *layout = new QVBoxLayout(m_chatWidget);
	layout->addWidget(view);
	layout->addWidget(chatInputWidget);
	layout->setMargin(0);

	m_view = qobject_cast<ChatViewWidget*>(view);

	loadSettings();
	connect(m_sessionList,SIGNAL(remove(ChatSessionImpl*)),SLOT(removeSession(ChatSessionImpl*)));
	connect(m_stack,SIGNAL(currentChanged(int)),SLOT(onCurrentChanged(int)));

	m_recieverList->setIcon(Icon("view-choose"));

	setAttribute(Qt::WA_AcceptTouchEvents);
	connect(m_stack, SIGNAL(fingerGesture(enum SlidingStackedWidget::SlideDirection)),
			this, SLOT(fingerGesture(enum SlidingStackedWidget::SlideDirection)));
	connect(m_stack, SIGNAL(animationFinished()),
			this, SLOT(animationFinished()));

	QAction *sendAct = new QAction(tr("Send"),m_chatWidget);
	sendAct->setSoftKeyRole(QAction::NegativeSoftKey);
	m_chatWidget->addAction(sendAct);
	connect(sendAct, SIGNAL(triggered()), m_chatInput, SLOT(send()));

	m_unitActions->setSoftKeyRole(QAction::PositiveSoftKey);
	m_chatWidget->addAction(m_unitActions);

	m_chatWidget->setVisible(false);
	m_sessionList->setVisible(false);
}

void StackedChatWidget::loadSettings()
{
	ConfigGroup cfg = Config("appearance").group("chat/behavior/widget");
	m_chatInput->setSendKey(cfg.value("sendKey", SendEnter));
}

StackedChatWidget::~StackedChatWidget()
{
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
	m_menu->addAction(gen);
}

void StackedChatWidget::addSession(ChatSessionImpl *session)
{
	if(!m_sessionList->count()) {
		m_sessionList->setVisible(true);
		m_chatWidget->setVisible(true);
		m_stack->addWidget(m_sessionList);
		m_stack->addWidget(m_chatWidget);
	}

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
	session->deleteLater();

	if(!m_sessionList->count()) {
			m_sessionList->setVisible(false);
			m_chatWidget->setVisible(false);
			m_stack->removeWidget(m_sessionList);
			m_stack->removeWidget(m_chatWidget);
	}

	m_stack->slideInIdx(m_stack->indexOf(m_sessionList));
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

	m_recieverList->setMenu(session->menu());
	//FIXME Symbian workaround
	m_chatWidget->removeAction(m_unitActions);
	m_unitActions->setMenu(session->getUnit()->menu());
	m_chatWidget->addAction(m_unitActions);

	m_stack->slideInIdx(m_stack->indexOf(m_chatWidget));
}

ChatSessionImpl *StackedChatWidget::currentSession() const
{
	return m_sessionList->currentSession();
}

bool StackedChatWidget::event(QEvent *event)
{
	if (event->type() == QEvent::TouchBegin) {

		event->accept();
		return true;
	}
	return AbstractChatWidget::event(event);
}

void StackedChatWidget::fingerGesture( enum SlidingStackedWidget::SlideDirection direction)
{
	if (direction==SlidingStackedWidget::LeftToRight) {
		m_stack->slideInPrev();
		m_contactView->blockSignals(true);
	}
	else if (direction==SlidingStackedWidget::RightToLeft) {
		m_stack->slideInNext();
		m_contactView->blockSignals(true);
	}

}

void StackedChatWidget::activateWindow()
{
	AbstractChatWidget::activateWindow();
}

void StackedChatWidget::onUnreadChanged()
{
	ChatSessionImpl *s = qobject_cast<ChatSessionImpl*>(sender());
	if(s && s == m_sessionList->currentSession())
		setTitle(s);
}

void StackedChatWidget::onCurrentChanged(int index)
{
	if(index != m_stack->indexOf(m_chatWidget))
		currentSession()->setActive(false);
}

void StackedChatWidget::animationFinished()
{
	m_contactView->blockSignals(false);
}

} // namespace Symbian
} // namespace AdiumChat
} // namespace Core

#include "tabbedchatwidget.h"
#include <qutim/servicemanager.h>
#include <chatlayer/chatviewfactory.h>
#include <QVBoxLayout>
#include <qutim/actiontoolbar.h>
#include "tabbar.h"
#include "chatedit.h"
#include <QPlainTextEdit>
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <qutim/conference.h>
#include <QAbstractItemModel>

namespace Core
{
namespace AdiumChat
{

TabbedChatWidget::TabbedChatWidget(const QString &key, QWidget *parent) :
	AbstractChatWidget(parent),
	m_toolbar(new ActionToolBar(tr("Chat Actions"),this)),
	m_tabbar(new TabBar(this)),
	m_chatInput(new ChatEdit(this))
{
	setAttribute(Qt::WA_DeleteOnClose);
	Q_UNUSED(key);
	QWidget *w = new QWidget(this);
	setCentralWidget(w);
	QWidget *view = ChatViewFactory::instance()->createViewWidget();
	view->setParent(w);

	QVBoxLayout *l = new QVBoxLayout(w);
	l->addWidget(m_tabbar);
	l->addWidget(view);
	l->addWidget(m_chatInput);

	addToolBar(Qt::TopToolBarArea,m_toolbar);
	m_toolbar->setMovable(false);
	m_toolbar->setMoveHookEnabled(true);
	setUnifiedTitleAndToolBarOnMac(true);

	m_tabbar->setTabsClosable(true);
	m_tabbar->setMovable(true);

	m_view = qobject_cast<ChatViewWidget*>(view);

	m_actSeparator = m_toolbar->addSeparator();
	m_unitSeparator = m_toolbar->addSeparator();

	connect(m_tabbar,SIGNAL(activate(ChatSessionImpl*)),SLOT(activate(ChatSessionImpl*)));
	connect(m_tabbar,SIGNAL(remove(ChatSessionImpl*)),SLOT(removeSession(ChatSessionImpl*)));
}

void TabbedChatWidget::loadSettings()
{

}

QPlainTextEdit *TabbedChatWidget::getInputField() const
{
	return m_chatInput;
}

bool TabbedChatWidget::contains(ChatSessionImpl *session) const
{
	return m_tabbar->contains(session);
}

void TabbedChatWidget::addAction(ActionGenerator *gen)
{
	m_toolbar->insertAction(m_actSeparator,gen);
}

void TabbedChatWidget::addSession(ChatSessionImpl *session)
{
	m_tabbar->addSession(session);
	connect(session,SIGNAL(activated(bool)),SLOT(onSessionActivated(bool)));
}

void TabbedChatWidget::removeSession(ChatSessionImpl *session)
{
	if(contains(session))
		m_tabbar->removeSession(session);
	//TODO delete on close flag
	session->setActive(false);
	session->deleteLater();

	if(!m_tabbar->count())
		deleteLater();
}

void TabbedChatWidget::onSessionActivated(bool active)
{
	if(!active)
		return;

	ChatSessionImpl *session = qobject_cast<ChatSessionImpl*>(sender());
	Q_ASSERT(session);

	m_tabbar->setCurrentSession(session);
}

void TabbedChatWidget::activate(ChatSessionImpl *session)
{
	if(m_currentSession) {
		if(m_currentSession == session)
			return;
		m_currentSession->setActive(false);
	}
	m_currentSession = session;

	m_view->setViewController(session->getController());
	m_tabbar->setCurrentSession(session);
	m_chatInput->setSession(session);

	ChatUnit *u = session->getUnit();
	QIcon icon = Icon("view-choose");
	QString title = tr("Chat with %1").arg(u->title());

	bool isContactsViewVisible;
	if (Conference *c = qobject_cast<Conference *>(u)) {
		icon = Icon("meeting-attending"); //TODO
		title = tr("Conference %1 (%2)").arg(c->title(),c->id());
		isContactsViewVisible = true;
	} else {
		isContactsViewVisible = session->getModel()->rowCount(QModelIndex()) > 0;
		if (Buddy *b = qobject_cast<Buddy*>(u))
			icon = b->avatar().isEmpty() ? Icon("view-choose") : QIcon(b->avatar());
	}
	setWindowTitle(title);
	setWindowIcon(icon);

	if(!session->unread().isEmpty())
		session->markRead();

	qDeleteAll(m_unitActions);
	m_unitActions.clear();
	ActionContainer container(session->getUnit(),ActionContainer::TypeMatch,ActionTypeChatButton);
	for (int i = 0;i!=container.count();i++) {
		QAction *current = container.action(i);
		m_toolbar->insertAction(m_unitSeparator,current);
		m_unitActions.append(current);
	}
}

ChatSessionImpl *TabbedChatWidget::currentSession() const
{
	return m_tabbar->currentSession();
}

TabbedChatWidget::~TabbedChatWidget()
{
	delete m_tabbar;
}

bool TabbedChatWidget::event(QEvent *event)
{
	if (event->type() == QEvent::WindowActivate
			|| event->type() == QEvent::WindowDeactivate) {
		bool active = event->type() == QEvent::WindowActivate;
		if (m_tabbar->currentSession())
			return false;
		m_tabbar->currentSession()->setActive(active);
	}
	return AbstractChatWidget::event(event);
}

}
}


#include "tabbedchatwidget.h"
#include <qutim/servicemanager.h>
#include <chatlayer/chatviewfactory.h>
#include <QVBoxLayout>
#include <qutim/actiontoolbar.h>
#include "tabbar.h"
#include "chatedit.h"
#include "conferencecontactsview.h"
#include <QPlainTextEdit>
#include <qutim/debug.h>
#include <qutim/icon.h>
#include <qutim/conference.h>
#include <qutim/config.h>
#include <QAbstractItemModel>
#include <QSplitter>


namespace Core
{
namespace AdiumChat
{

TabbedChatWidget::TabbedChatWidget(const QString &key, QWidget *parent) :
	AbstractChatWidget(parent),
	m_toolbar(new ActionToolBar(tr("Chat Actions"),this)),
	m_tabbar(new TabBar(this)),
	m_chatInput(new ChatEdit(this)),
	m_recieverList(new QAction(Icon("view-choose"),tr("Destination"),this)),
	m_contactView(new ConferenceContactsView(this)),
	m_key(key)
{
	setAttribute(Qt::WA_DeleteOnClose);	
	QWidget *w = new QWidget(this);
	setCentralWidget(w);
	QWidget *view = ChatViewFactory::instance()->createViewWidget();
	view->setParent(w);

	QSplitter *hSplitter = new QSplitter(Qt::Horizontal,this);
	hSplitter->setObjectName(QLatin1String("hSplitter"));
	hSplitter->addWidget(view);
	hSplitter->addWidget(m_contactView);

	QSplitter *vSplitter = new QSplitter(Qt::Vertical,this);
	vSplitter->setObjectName(QLatin1String("vSplitter"));
	vSplitter->addWidget(hSplitter);
	vSplitter->addWidget(m_chatInput);

	QVBoxLayout *l = new QVBoxLayout(w);
	l->addWidget(m_tabbar);
	l->addWidget(vSplitter);

	addToolBar(Qt::TopToolBarArea,m_toolbar);
	m_toolbar->setMovable(false);
	m_toolbar->setMoveHookEnabled(true);
	setUnifiedTitleAndToolBarOnMac(true);

	m_tabbar->setTabsClosable(true);
	m_tabbar->setMovable(true);

	m_view = qobject_cast<ChatViewWidget*>(view);

	m_actSeparator = m_toolbar->addSeparator();
	m_unitSeparator = m_toolbar->addSeparator();

	//simple hack
	m_recieverList->setMenu(new QMenu);
	m_toolbar->addAction(m_recieverList);
	m_recieverList->menu()->deleteLater();

	QWidget* spacer = new QWidget(this);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	spacer->setAttribute(Qt::WA_TransparentForMouseEvents);
	m_toolbar->addWidget(spacer);
	QAction *a = new QAction(Icon("view-list-tree"),tr("Session list"),this);
	a->setMenu(m_tabbar->menu());
	m_toolbar->addAction(a);
	loadSettings();

	connect(m_tabbar,SIGNAL(remove(ChatSessionImpl*)),SLOT(removeSession(ChatSessionImpl*)));
}

void TabbedChatWidget::loadSettings()
{
	ConfigGroup cfg = Config("appearance");
	if(!property("loaded").toBool()) {
		ConfigGroup keyGroup = cfg.group("keys");
		if (keyGroup.hasChildGroup(m_key)) {
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
			centerizeWidget(this);
		}
		setProperty("loaded",true);
	}
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
	session->setActive(false);
	//TODO delete on close flag
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
	m_contactView->setSession(session);

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
	m_recieverList->setMenu(session->menu());
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
	ConfigGroup group = Config("appearance").group("chat/behavior/widget/keys").group(m_key);
	group.setValue("geometry", saveGeometry());
	foreach (QSplitter *splitter, findChildren<QSplitter*>()) {
		group.setValue(splitter->objectName(), splitter->saveState());
	}
	group.sync();
}

bool TabbedChatWidget::event(QEvent *event)
{
	if (event->type() == QEvent::WindowActivate
			|| event->type() == QEvent::WindowDeactivate) {
		bool active = event->type() == QEvent::WindowActivate;
		if (!m_tabbar->currentSession())
			return false;
		m_tabbar->currentSession()->setActive(active);
	}
	return AbstractChatWidget::event(event);
}

}
}


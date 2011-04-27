#include "macwidget.h"
#include <abstractcontactmodel.h>
#include <simplestatusdialog.h>
#include <qutim/account.h>
#include <qutim/actiongenerator.h>
#include <qutim/actiontoolbar.h>
#include <qutim/config.h>
#include <qutim/contact.h>
#include <qutim/icon.h>
#include <qutim/messagesession.h>
#include <qutim/metacontact.h>
#include <qutim/protocol.h>
#include <qutim/qtwin.h>
#include <qutim/servicemanager.h>
#include <qutim/shortcut.h>
#include <qutim/systemintegration.h>
#include <qutim/utils.h>
#include <QAbstractItemDelegate>
#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QTimer>

namespace Core {
namespace SimpleContactList {
struct MacWidgetPrivate
{
	TreeView *view;
	AbstractContactModel *model;
	ActionToolBar *mainToolBar;
	QLineEdit *searchBar;
	QAction *statusTextAction;
	QHash<QString, MenuController *> menu;
	QHash<Account *, QAction *> accountActions;
	QHash<ChatSession *, QAction *> aliveSessions;
	QMenuBar *menuBar;
};

MacWidget::MacWidget() : d_ptr(new MacWidgetPrivate())
{
	Q_D(MacWidget);
	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(deleteLater()));
	setWindowIcon(Icon("qutim"));

	resize(150,0);//hack
	setAttribute(Qt::WA_AlwaysShowToolTips);
	loadGeometry();

	QWidget *w = new QWidget(this);
	setCentralWidget(w);
	setUnifiedTitleAndToolBarOnMac(true);

	QVBoxLayout *layout = new QVBoxLayout(w);
	layout->setMargin(0);
	layout->setSpacing(0);

	if (QtWin::isCompositionEnabled()) {
		QtWin::extendFrameIntoClientArea(this);
		setContentsMargins(0, 0, 0, 0);
	}

	int size = 22;
	size = Config().group("contactList").value("toolBarIconSize", size);

	QSize toolbar_size (size,size);
	d->mainToolBar = new ActionToolBar(this);
	d->mainToolBar->setWindowTitle(tr("Main Toolbar"));
	addToolBar(Qt::TopToolBarArea, d->mainToolBar);

	d->mainToolBar->setIconSize(toolbar_size);
	d->mainToolBar->setFloatable(false);
	d->mainToolBar->setMovable(false);
	d->mainToolBar->setMoveHookEnabled(true);
	d->mainToolBar->setObjectName(QLatin1String("contactListBar"));

	d->model = ServiceManager::getByName<AbstractContactModel *>("ContactModel");
	d->view = new TreeView(d->model, this);
	layout->addWidget(d->view);
	d->view->setItemDelegate(ServiceManager::getByName<QAbstractItemDelegate *>("ContactDelegate"));
	d->view->setAlternatingRowColors(false);
	//d->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	d->searchBar = new QLineEdit(this);
	QWidgetAction *widgetAction = new QWidgetAction(this);
	widgetAction->setDefaultWidget(d->searchBar);
	d->mainToolBar->addAction(widgetAction);
	connect(d->searchBar, SIGNAL(textChanged(QString)), d->model, SLOT(filterList(QString)));

	qApp->setAttribute(Qt::AA_DontShowIconsInMenus);
#ifdef Q_OS_MAC
	d->menuBar = new QMenuBar(0);
#else
	d->menuBar = new QMenuBar(this);
#endif
	addMenu(tr("File"), "file");
	addMenu(tr("Accounts"), "accounts");
	addMenu(tr("Chats"), "chats");
	addMenu(tr("Roster"), "roster");
	//if (MenuController *contactList = qobject_cast<MenuController *>(ServiceManager::getByName("ContactList")))
	//	d->menu.value("file")->setMenuOwner(contactList);
	d->statusTextAction = d->menu.value("accounts")->menu()->addAction(Icon("im-status-message-edit"), tr("Set Status Text"),
																	   this, SLOT(showStatusDialog()));
	QString lastStatus = Config().group("contactList").value("lastStatus", QString());
	d->statusTextAction->setData(lastStatus);
	d->menu.value("accounts")->menu()->addSeparator();
	foreach(Protocol *protocol, Protocol::all())
		connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account *)), this, SLOT(onAccountCreated(qutim_sdk_0_3::Account *)));
	QTimer timer;
	timer.singleShot(0, this, SLOT(initMenu()));
}

MacWidget::~MacWidget()
{
	Config config;
	config.beginGroup("contactList");
	config.setValue("geometry", saveGeometry());
}

void MacWidget::addMenu(const QString &title, const QString &id)
{
	Q_D(MacWidget);
	MenuController *controller = new MenuController(this);
	controller->menu()->setTitle(title);
	d->menuBar->addMenu(controller->menu());
	d->menu.insert(id, controller);
}

void MacWidget::addButton(ActionGenerator *generator)
{
	d_func()->menu.value("roster")->addAction(generator);
}

void MacWidget::removeButton(ActionGenerator *generator)
{
	d_func()->menu.value("roster")->removeAction(generator);
}

void MacWidget::loadGeometry()
{
	QByteArray geom = Config().group("contactList").value("geometry", QByteArray());
	if (geom.isNull()) {
		QRect rect = QApplication::desktop()->availableGeometry(QCursor::pos());
		//black magic
		int width = size().width();
		int x = rect.width() - width;
		int y = 0;
		int height = rect.height();
		QRect geometry(x, y, width, height);
		setGeometry(geometry);
	} else {
		restoreGeometry(geom);
	}
}

void MacWidget::showStatusDialog()
{
	QString text = d_func()->statusTextAction->data().toString();
	SimpleStatusDialog *dialog = new SimpleStatusDialog(text, this);
	connect(dialog, SIGNAL(accepted()), SLOT(changeStatusTextAccepted()));
	centerizeWidget(dialog);
	SystemIntegration::show(dialog);
}

void MacWidget::changeStatusTextAccepted()
{
	SimpleStatusDialog *dialog = qobject_cast<SimpleStatusDialog *>(sender());
	Q_ASSERT(dialog);
	QString text = dialog->statusText();
	d_func()->statusTextAction->setData(text);
	foreach(Protocol *proto, Protocol::all()) {
		foreach(Account *account, proto->accounts()) {
			Status status = account->status();
			status.setText(text);
			account->setStatus(status);
		}
	}
	Config config = Config().group("contactList");
	config.setValue("lastStatus",text);
	config.sync();
}

void MacWidget::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	Q_D(MacWidget);
	QAction *action = new QAction(account->status().icon(), account->id(), this);
	action->setIconVisibleInMenu(true);
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
	connect(account, SIGNAL(destroyed(QObject *)),SLOT(onAccountDestroyed(QObject *)));
	d->accountActions.insert(account, action);
	action->setMenu(account->menu());
	d->menu.value("accounts")->menu()->addAction(action);
	QString text = d->statusTextAction->data().toString();
	if (!text.isEmpty()) {
		Status status = account->status();
		status.setText(text);
		account->setStatus(status);
	}
}

void MacWidget::onAccountStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Q_D(MacWidget);
	Account *account = qobject_cast<Account *>(sender());
	Q_ASSERT(account);
	QAction *action = d->accountActions.value(account);
	Q_ASSERT(action);
	action->setIcon(status.icon());
}

void MacWidget::onAccountDestroyed(QObject *obj)
{
	Account *account = reinterpret_cast<Account *>(obj);
	d_func()->accountActions.take(account)->deleteLater();
}

void MacWidget::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	Q_D(MacWidget);
	QAction *action = new QAction(session->getUnit()->title(), d->menu.value("chats")->menu());
	action->setCheckable(true);
	connect(action, SIGNAL(triggered()), session, SLOT(activate()));
	d->menu.value("chats")->menu()->addAction(action);
	d->aliveSessions.insert(session, action);
	connect(session, SIGNAL(activated(bool)), this, SLOT(onActivatedSession(bool)));
	connect(session, SIGNAL(destroyed()), SLOT(onSessionDestroyed()));
}

void MacWidget::onSessionDestroyed()
{
	ChatSession *session = static_cast<ChatSession *>(sender());
	delete d_func()->aliveSessions.take(session);
}

void MacWidget::onActivatedSession(bool state)
{
	ChatSession *session = static_cast<ChatSession *>(sender());
	d_func()->aliveSessions.value(session)->setChecked(state);
}

void MacWidget::initMenu()
{
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession *)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession *)));
	if (MenuController *cl = ServiceManager::getByName<MenuController *>("ContactList"))
		d_func()->menu.value("file")->menu()->addActions(cl->menu()->actions());
}
} // namespace SimpleContactList
} // namespace Core


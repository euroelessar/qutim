#include "torywidget.h"
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
#include <QHBoxLayout>
#include <QToolButton>
#include <QWidgetAction>
#include <QTimer>
#include <QMouseEvent>
#include <qutim/debug.h>

namespace Core {
namespace SimpleContactList {
struct ToryWidgetPrivate
{
	TreeView *view;
	AbstractContactModel *model;
	ActionToolBar *mainToolBar;
	QHBoxLayout *accountsContainer;
	QLineEdit *searchBar;
	QAction *globalStatus;
	QAction *statusTextAction;
	QHash<Account *, QToolButton *> accounts;
	QActionGroup *statusGroup;
};

ToryWidget::ToryWidget() : d_ptr(new ToryWidgetPrivate())
{
	Q_D(ToryWidget);
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

	d->globalStatus = new QAction(Icon("im-user-offline"), tr("Global status"), this);
	d->statusTextAction = new QAction(Icon("im-status-message-edit"), tr("Set Status Text"), this);
	connect(d->statusTextAction, SIGNAL(triggered()), SLOT(showStatusDialog()));
	QMenu *globMenu = new QMenu(this);
	d->globalStatus->setMenuRole(QAction::PreferencesRole);
	globMenu->addAction(d->statusTextAction);
	globMenu->addSeparator();
	d->statusGroup = new QActionGroup(this);
	globMenu->addAction(createGlobalStatus(Status::Online));
	globMenu->addAction(createGlobalStatus(Status::FreeChat));
	globMenu->addAction(createGlobalStatus(Status::Away));
	globMenu->addAction(createGlobalStatus(Status::DND));
	globMenu->addAction(createGlobalStatus(Status::NA));
	globMenu->addAction(createGlobalStatus(Status::Invisible));
	globMenu->addAction(createGlobalStatus(Status::Offline));
	d->mainToolBar->addAction(d->globalStatus);
	d->globalStatus->setMenu(globMenu);

	// Get rid of the delay before the action menu pops up
	// TODO: it should probably be moved to ActionToolBar
	QToolButton *button = qobject_cast<QToolButton*>(d->mainToolBar->widgetForAction(d->globalStatus));
	if (button) {
		button->setPopupMode(QToolButton::InstantPopup);
	}

	QString lastStatus = Config().group("contactList").value("lastStatus", QString());
	d->statusTextAction->setData(lastStatus);

	QAction *searchBtn = new QAction(Icon("edit-find"), tr("Search contact"), this);
	searchBtn->setCheckable(true);
	d->searchBar = new QLineEdit(this);
	d->searchBar->setVisible(false);
	connect(searchBtn, SIGNAL(toggled(bool)), SLOT(onSearchButtonToggled(bool)));
	d->mainToolBar->addAction(searchBtn);
	layout->insertWidget(0, d->searchBar);
	connect(d->searchBar, SIGNAL(textChanged(QString)), d->model, SLOT(filterList(QString)));

	foreach(Protocol *protocol, Protocol::all())
		connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account *)), this, SLOT(onAccountCreated(qutim_sdk_0_3::Account *)));
	QTimer timer;
	timer.singleShot(0, this, SLOT(initMenu()));
}

ToryWidget::~ToryWidget()
{
	Config config;
	config.beginGroup("contactList");
	config.setValue("geometry", saveGeometry());
}

void ToryWidget::addButton(ActionGenerator *generator)
{
	d_func()->mainToolBar->addAction(generator);
}

void ToryWidget::removeButton(ActionGenerator *)
{
	//d_func()->mainToolBar->removeAction(generator);
}

QAction *ToryWidget::createGlobalStatus(Status::Type type)
{
	Status s = Status(type);
	QAction *act = new QAction(s.icon(), s.name(), this);
	connect(act, SIGNAL(triggered(bool)), SLOT(onStatusChanged()));
	d_func()->statusGroup->addAction(act);
	act->setData(type);
	return act;
}

void ToryWidget::loadGeometry()
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

void ToryWidget::showStatusDialog()
{
	QString text = d_func()->statusTextAction->data().toString();
	SimpleStatusDialog *dialog = new SimpleStatusDialog(text, this);
	connect(dialog, SIGNAL(accepted()), SLOT(changeStatusTextAccepted()));
	centerizeWidget(dialog);
	SystemIntegration::show(dialog);
}

void ToryWidget::changeStatusTextAccepted()
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

void ToryWidget::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	Q_D(ToryWidget);
	if (!d->accountsContainer) {
		QWidget *accountsWidget = new QWidget(this);
		d->accountsContainer = new QHBoxLayout(accountsWidget);
		d->accountsContainer->setMargin(0);
		d->accountsContainer->setSpacing(0);
		if (QLayout *layout = centralWidget()->layout())
			layout->addWidget(accountsWidget);
		accountsWidget->installEventFilter(this);
	}
	QToolButton *button = new QToolButton(this);
	button->setMenu(account->menu());
	button->setIcon(account->status().icon());
	button->setToolTip(account->id());
	button->setAutoRaise(true);
	button->installEventFilter(this);

	d->accountsContainer->addWidget(button);
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
	connect(account, SIGNAL(destroyed(QObject *)),SLOT(onAccountDestroyed(QObject *)));
	d->accounts.insert(account, button);
	QString text = d->statusTextAction->data().toString();
	if (!text.isEmpty()) {
		Status status = account->status();
		status.setText(text);
		account->setStatus(status);
	}
}

void ToryWidget::onAccountStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Q_D(ToryWidget);
	Account *account = qobject_cast<Account *>(sender());
	Q_ASSERT(account);
	QToolButton *button = d->accounts.value(account);
	Q_ASSERT(button);
	button->setIcon(status.icon());
	bool isOnline = false;
	foreach(qutim_sdk_0_3::Account *account, qutim_sdk_0_3::Account::all()) {
		Status::Type type = account->status().type();
		if (type != Status::Offline && type != Status::Connecting) {
			isOnline = true;
			break;
		}
	}
	d->globalStatus->setIcon(Icon(isOnline ? "im-user-online" : "im-user-offline"));
}

void ToryWidget::onAccountDestroyed(QObject *obj)
{
	Account *account = reinterpret_cast<Account *>(obj);
	d_func()->accounts.take(account)->deleteLater();
}

void ToryWidget::onSearchButtonToggled(bool toggled)
{
	Q_D(ToryWidget);
	d->searchBar->setVisible(toggled);
	if (toggled)
		d->searchBar->setFocus(Qt::PopupFocusReason);
	else
		d->searchBar->clear();
}

void ToryWidget::onStatusChanged()
{
	if (QAction *a = qobject_cast<QAction *>(sender())) {
		Status::Type type = static_cast<Status::Type>(a->data().value<int>());
		foreach(qutim_sdk_0_3::Protocol *proto, qutim_sdk_0_3::Protocol::all()) {
			foreach(Account *account, proto->accounts()) {
				Status status = account->status();
				status.setType(type);
				status.setSubtype(0);
				status.setProperty("changeReason",Status::ByUser);
				account->setStatus(status);
			}
		}
	}
}

bool ToryWidget::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::MouseButtonPress) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

		QMenu *menu = 0;
		if (mouseEvent->button() == Qt::RightButton)
			menu = d_func()->globalStatus->menu();
		else if (QToolButton *btn = qobject_cast<QToolButton*>(obj))
			menu = btn->menu();

		if (menu) {
			menu->popup(mouseEvent->globalPos());
			return true;
		}
		return QMainWindow::eventFilter(obj, event);
	} else {
		return QMainWindow::eventFilter(obj, event);
	}
}


void ToryWidget::initMenu()
{
	Q_D(ToryWidget);
	MenuController *controller = ServiceManager::getByName<MenuController*>("ContactList");
	ActionGenerator *gen = new MenuActionGenerator(Icon("show-menu"), QByteArray(), controller);
	gen->setShortcut(Shortcut::getSequence("contactListActivateMainMenu").key);
	QAction *before = d->mainToolBar->actions().count() ? d->mainToolBar->actions().first() : 0;
	d->mainToolBar->insertAction(before, gen);
	SystemIntegration::show(this);
}

void ToryWidget::onActivatedSession(bool state)
{

}

void ToryWidget::onSessionCreated(qutim_sdk_0_3::ChatSession*)
{

}

void ToryWidget::onSessionDestroyed()
{

}

} // namespace SimpleContactList
} // namespace Core


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
#include "torywidget.h"
#include <qutim/simplecontactlist/simplestatusdialog.h>
#include <qutim/account.h>
#include <qutim/accountmanager.h>
#include <qutim/actiongenerator.h>
#include <qutim/actiontoolbar.h>
#include <qutim/config.h>
#include <qutim/contact.h>
#include <qutim/icon.h>
#include <qutim/chatsession.h>
#include <qutim/metacontact.h>
#include <qutim/protocol.h>
#include <qutim/qtwin.h>
#include <qutim/servicemanager.h>
#include <qutim/shortcut.h>
#include <qutim/systemintegration.h>
#include <qutim/utils.h>
#include <qutim/statusactiongenerator.h>
#include <QAbstractItemDelegate>
#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <qutim/simplecontactlist/lineedit.h>
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
class ToryWidgetPrivate
{
public:
	ToryWidgetPrivate() : model("ContactModel"), accountsContainer(NULL) {}

	TreeView *view;
	ServicePointer<QAbstractItemModel> model;
	ActionToolBar *mainToolBar;
	QHBoxLayout *accountsContainer;
	QLineEdit *searchBar;
	QAction *globalStatus;
	QAction *statusTextAction;
	QHash<Account *, QToolButton *> accounts;
	QActionGroup *statusGroup;
	QList<ActionGenerator*> actionGenerators;
};

ToryWidget::ToryWidget() : d_ptr(new ToryWidgetPrivate())
{
	if (1) {} else Q_UNUSED(QT_TRANSLATE_NOOP("ContactList", "qutIM 0.2 style"));
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
#ifdef Q_OS_WIN
	d->mainToolBar->setStyleSheet("QToolBar{background:none;border:none;}"); //HACK
#endif

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
    d->searchBar = new LineEdit(this);
	d->searchBar->setVisible(false);
	connect(searchBtn, SIGNAL(toggled(bool)), SLOT(onSearchButtonToggled(bool)));
	d->mainToolBar->addAction(searchBtn);
	layout->insertWidget(0, d->searchBar);
	connect(d->searchBar, SIGNAL(textChanged(QString)), d->model, SLOT(setFilterFixedString(QString)));

	connect(ServiceManager::instance(),
			SIGNAL(serviceChanged(QByteArray,QObject*,QObject*)),
			SLOT(onServiceChanged(QByteArray,QObject*,QObject*)));

	foreach(Protocol *protocol, Protocol::all()) {
		foreach (Account *account, protocol->accounts())
			onAccountCreated(account);
		connect(protocol, SIGNAL(accountCreated(qutim_sdk_0_3::Account *)),
				this, SLOT(onAccountCreated(qutim_sdk_0_3::Account *)));
	}
	QTimer timer;
	timer.singleShot(0, this, SLOT(initMenu()));
}

ToryWidget::~ToryWidget()
{
	Config config;
	config.beginGroup("contactList");
	config.setValue("geometry", saveGeometry());
	qDeleteAll(d_func()->statusGroup->actions());
	qDeleteAll(d_func()->actionGenerators);
}

void ToryWidget::addButton(ActionGenerator *generator)
{
	d_func()->mainToolBar->addAction(generator);
}

void ToryWidget::removeButton(ActionGenerator *generator)
{
	d_func()->mainToolBar->removeAction(generator);
}

TreeView *ToryWidget::contactView()
{
	return d_func()->view;
}

QAction *ToryWidget::createGlobalStatus(Status::Type type)
{
	Q_D(ToryWidget);
	ActionGenerator *generator = new StatusActionGenerator(Status(type));
	QAction *action = generator->generate<QAction>();
	connect(action, SIGNAL(triggered(bool)), SLOT(onStatusChanged()));
	d->actionGenerators << generator;
	d->statusGroup->addAction(action);
	action->setData(type);
	return action;
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
			account->setUserStatus(status);
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
		QSpacerItem *horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
		d->accountsContainer->addItem(horizontalSpacer);
		if (QLayout *layout = centralWidget()->layout())
			layout->addWidget(accountsWidget);
		accountsWidget->installEventFilter(this);
	}
	QToolButton *button = new QToolButton(this);
	QMenu *menu = account->menu(false);
	connect(button, SIGNAL(destroyed()), menu, SLOT(deleteLater()));
	button->setMenu(menu);
	button->setIcon(account->status().icon());
	button->setToolTip(account->id());
	button->setAutoRaise(true);
	button->setPopupMode(QToolButton::InstantPopup);
	button->setStyleSheet("QToolButton::menu-indicator{border:none}"); // remove arrow
	button->setMaximumSize(22, 22);
	button->setMinimumSize(22, 22);

	d->accountsContainer->addWidget(button);
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
	connect(account, SIGNAL(destroyed(QObject *)),SLOT(onAccountDestroyed(QObject *)));
	d->accounts.insert(account, button);
	QString text = d->statusTextAction->data().toString();
	if (!text.isEmpty()) {
		Status status = account->status();
		status.setText(text);
		account->setUserStatus(status);
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
	foreach (Account *account, AccountManager::instance()->accounts()) {
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
				status.setChangeReason(Status::ByUser);
				account->setUserStatus(status);
			}
		}
	}
}

bool ToryWidget::event(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		Q_D(ToryWidget);
		d->globalStatus->setText(tr("Global status"));
		d->statusTextAction->setText(tr("Set Status Text"));
		event->accept();
	}
	return QMainWindow::event(event);
}

bool ToryWidget::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::MouseButtonPress) {
		QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
		QMenu *menu = 0;
		if (mouseEvent->button() == Qt::RightButton) {
			menu = d_func()->globalStatus->menu();
			menu->popup(mouseEvent->globalPos());
			return true;
		}
	}
	return QMainWindow::eventFilter(obj, event);
}


void ToryWidget::initMenu()
{
	Q_D(ToryWidget);
	MenuController *controller = ServiceManager::getByName<MenuController*>("ContactList");
	ActionGenerator *gen = new MenuActionGenerator(Icon("show-menu"), QByteArray(), controller);
	gen->setShortcut(QLatin1String("contactListActivateMainMenu"));
	QAction *before = d->mainToolBar->actions().count() ? d->mainToolBar->actions().first() : 0;
	d->mainToolBar->insertAction(before, gen);
	Config config("appearance");
	config.beginGroup("contactList");
	if(config.value<bool>("showContactListOnStartup", true))
		SystemIntegration::show(this);
}

void ToryWidget::onServiceChanged(const QByteArray &name, QObject *now, QObject *old)
{
	Q_D(ToryWidget);
	Q_UNUSED(old);
	if (name == "ContactModel") {
		d->view->setContactModel(d->model);
		connect(d->searchBar, SIGNAL(textChanged(QString)), d->model, SLOT(setFilterFixedString(QString)));
	} else if (name == "ContactDelegate") {
		d->view->setItemDelegate(sender_cast<QAbstractItemDelegate*>(now));
	}
}

void ToryWidget::onActivatedSession(bool state)
{
	Q_UNUSED(state);
}

void ToryWidget::onSessionCreated(qutim_sdk_0_3::ChatSession*)
{

}

void ToryWidget::onSessionDestroyed()
{

}

} // namespace SimpleContactList
} // namespace Core


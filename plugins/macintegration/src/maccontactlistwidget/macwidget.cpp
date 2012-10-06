/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Denis Daschenko <daschenko@gmail.com>
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

#include "macwidget_p.h"
#include <qutim/simplecontactlist/abstractcontactmodel.h>
#include <qutim/simplecontactlist/simplestatusdialog.h>
#include <qutim/account.h>
#include <qutim/actiongenerator.h>
#include <qutim/actiontoolbar.h>
#include <qutim/config.h>
#include <qutim/contact.h>
#include <qutim/icon.h>
#include <qutim/chatsession.h>
#include <qutim/metacontact.h>
#include <qutim/protocol.h>
#include <qutim/qtwin.h>
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
#include <QKeyEvent>
#include "macsearchfield.h"

namespace Core {
namespace SimpleContactList {

class MacWidgetPrivate
{
public:
	TreeView *view;
    AbstractContactModel *model;
    MacSearchField *searchBar;
	QToolBar *toolBar;
    QAction *statusTextAction;
    QVector<MenuController*> controllers;
    QVector<QMenu*> menus;
    QHash<Account*, QAction*> accountActions;
    QHash<ChatSession*, QAction*> aliveSessions;
    QMenuBar *menuBar;
    QString pressedKeys;
};

MacWidget::MacWidget() : d_ptr(new MacWidgetPrivate())
{
    Q_D(MacWidget);
    d->controllers.resize(MacMenuSize);
    d->menus.resize(MacMenuSize);
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(deleteLater()));
    setWindowIcon(Icon("qutim"));

    setAttribute(Qt::WA_AlwaysShowToolTips);
    loadGeometry();

    QWidget *w = new QWidget(this);
    setCentralWidget(w);
    setUnifiedTitleAndToolBarOnMac(true);

    QVBoxLayout *layout = new QVBoxLayout(w);
    layout->setMargin(1);
    layout->setSpacing(0);

    Config cfg;
	cfg.beginGroup("contactlist");

    d->model = ServiceManager::getByName<AbstractContactModel *>("ContactModel");
    d->view = new TreeView(d->model, this);
    layout->addWidget(d->view);
    d->view->setItemDelegate(ServiceManager::getByName<QAbstractItemDelegate *>("ContactDelegate"));
    d->view->setAlternatingRowColors(cfg.value("alternatingRowColors", false));
    d->view->setFrameShape(QFrame::NoFrame);
    d->view->setFrameShadow(QFrame::Plain);
    d->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	d->toolBar = addToolBar(tr("Search"));
	QWidget *toolBarWidget = new QWidget(this);
	toolBarWidget->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred));
	QVBoxLayout *toolBarLayout = new QVBoxLayout(toolBarWidget);
	toolBarLayout->setMargin(0);
	d->toolBar->addWidget(toolBarWidget);

	d->searchBar = new MacSearchField(this);
	toolBarLayout->addWidget(d->searchBar);

	connect(d->searchBar, SIGNAL(textChanged(QString)), d->model, SLOT(filterList(QString)));
	connect(d->searchBar, SIGNAL(textChanged(QString)), this, SLOT(onTextChanged(QString)));
	d->searchBar->installEventFilter(this);
	d->searchBar->hide();
	// TEMP
	d->toolBar->hide();

	d->view->installEventFilter(this);

    qApp->setAttribute(Qt::AA_DontShowIconsInMenus);
#ifdef Q_OS_MAC
    d->menuBar = new QMenuBar(0);
#else
    d->menuBar = new QMenuBar(this);
#endif
    addMenu(tr("Accounts"), MacMenuAccounts);
    addMenu(tr("Chats"), MacMenuChats);

    d->statusTextAction = d->menus[MacMenuAccounts]->addAction(Icon("im-status-message-edit"), tr("Set Status Text"),
                                                               this, SLOT(showStatusDialog()));
    QString lastStatus = Config().group("contactList").value("lastStatus", QString());
    d->statusTextAction->setData(lastStatus);
    d->menus[MacMenuAccounts]->addSeparator();
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

void MacWidget::addMenu(const QString &title, MacMenuId id)
{
    Q_D(MacWidget);
    MenuController *controller = 0;
    if (id == MacMenuFile)
        controller = new FileMenuController(this);
    else if (id == MacMenuRoster)
        controller = new RosterMenuController(this);
    else
        controller = new MenuController(this);
    QMenu *menu = controller->menu(false);
    menu->setTitle(title);
    connect(this, SIGNAL(destroyed()), menu, SLOT(deleteLater()));
    d->menus[id] = menu;
    d->controllers[id] = controller;
}

void MacWidget::addButton(ActionGenerator *generator)
{
    MenuController::addAction<RosterMenuController>(generator);
}

void MacWidget::removeButton(ActionGenerator *generator)
{
    d_func()->controllers[MacMenuRoster]->removeAction(generator);
}

void MacWidget::loadGeometry()
{
    QByteArray geom = Config().group("contactList").value("geometry", QByteArray());
    if (!geom.isNull())
        restoreGeometry(geom);
	else
		resize(200, 600);
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
    action->setMenu(account->menu());
    d->menus[MacMenuAccounts]->addAction(action);
    d->accountActions.insert(account, action);
    QString text = d->statusTextAction->data().toString();
    if (!text.isEmpty()) {
        Status status = account->status();
        status.setText(text);
        account->setStatus(status);
    }

    connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
            this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
    connect(account, SIGNAL(destroyed(QObject *)),SLOT(onAccountDestroyed(QObject *)));
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
    QAction *action = new QAction(session->getUnit()->title(), d->menus[MacMenuChats]);
    action->setCheckable(true);
    connect(action, SIGNAL(triggered()), session, SLOT(activate()));
    d->menus[MacMenuChats]->addAction(action);
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
    Q_D(MacWidget);
    addMenu(tr("File"), MacMenuFile);
    addMenu(tr("Roster"), MacMenuRoster);

    d->menuBar->addMenu(d->menus[MacMenuFile]);
    d->menuBar->addMenu(d->menus[MacMenuAccounts]);
    d->menuBar->addMenu(d->menus[MacMenuChats]);
    d->menuBar->addMenu(d->menus[MacMenuRoster]);
    connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession *)),
            this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession *)));
}

bool MacWidget::eventFilter(QObject *obj, QEvent *ev)
{
    Q_D(MacWidget);
    if (obj == d->view) {
        if (ev->type() == QEvent::KeyPress) {
			QKeyEvent *event = static_cast<QKeyEvent*>(ev);
			if (event->key() == Qt::Key_Backspace)
				d->pressedKeys.chop(1);
			else if (d->view->hasFocus())
                d->pressedKeys.append(event->text());

			if (d->pressedKeys.count() > 1) {
				d->searchBar->show();
				// TEMP
				d->toolBar->show();
				d->searchBar->setFocus();
                d->searchBar->setText(d->pressedKeys);
				d->pressedKeys.clear();
			}
            ev->accept();
        } else if (ev->type() == QEvent::FocusOut && d->searchBar->isHidden()) {
            d->pressedKeys.clear();
        }
    } else if (obj == d->searchBar) {
		if (ev->type() == QEvent::FocusOut) {
			d->pressedKeys.clear();
			d->searchBar->setText(QString());
			d->searchBar->hide();
			// TEMP
			d->toolBar->hide();
		} else if (ev->type() == QEvent::FocusIn) {
			d->pressedKeys.clear();
		}
	}
    return QMainWindow::eventFilter(obj, ev);
}

void MacWidget::onTextChanged(const QString &text)
{
	d_func()->searchBar->setVisible(!text.isEmpty());
	// TEMP
	d_func()->toolBar->setVisible(!text.isEmpty());
}

} // namespace SimpleContactList
} // namespace Core


/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Aleksey Sidorov <sauron@citadelspb.com>
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

#include "sevenwidget.h"
#include <QPushButton>
#include <QAction>
#include <qutim/qtwin.h>

#include <qutim/simplecontactlist/abstractcontactmodel.h>
#include <qutim/simplecontactlist/simplestatusdialog.h>

#include <qutim/servicemanager.h>
#include <QAbstractItemDelegate>
#include <QHBoxLayout>
#include <qutim/icon.h>
#include <qutim/config.h>
#include <qutim/utils.h>
#include <qutim/actiontoolbar.h>
#include <qutim/account.h>
#include <qutim/contact.h>
#include <qutim/systemintegration.h>
#include <qutim/protocol.h>
#include <qutim/shortcut.h>
#include <QApplication>
#include <QLineEdit>
#include <qutim/metacontact.h>
#include <QDesktopWidget>
#include <qutim/actiongenerator.h>
#include <QTimer>
#include "toolframewindow.h"
#include <QToolButton>
#include <QMenuBar>

namespace Core {
namespace SimpleContactList {

static bool isStatusChange(const qutim_sdk_0_3::Status &status)
{
	if (status.type() == Status::Offline) {
		foreach(Protocol *proto, Protocol::all()) {
			foreach(Account *a, proto->accounts()) {
				debug() << a->status().name() << a->status().type();
				if (a->status().type()!=Status::Offline)
					return false;
			}
		}
		return true;
	}
	else
		return true;
}

SevenWidget::SevenWidget()
{
	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(save()));
	setWindowIcon(Icon("show-menu"));

	resize(150,0);//hack
	setAttribute(Qt::WA_AlwaysShowToolTips);
	loadGeometry();

	m_toolFrameWindow = new ToolFrameWindow(ToolFrameWindow::DisableExtendFrame);

	m_toolFrameWindow->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
	QtWin::extendFrameIntoClientArea(m_toolFrameWindow);
	m_toolFrameWindow->setCentralWidget(this);

	m_controller = new MenuController(this);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);

	m_model = ServiceManager::getByName<AbstractContactModel*>("ContactModel");
	Q_ASSERT(m_model);
	m_view = new TreeView(m_model, this);
	layout->addWidget(m_view);
	m_view->setItemDelegate(ServiceManager::getByName<QAbstractItemDelegate*>("ContactDelegate"));

	QMenu *statusMenu = new QMenu(this);


	m_searchBar = new QLineEdit(this);

	// make shortcuts
	Shortcut *key = new Shortcut("find", m_searchBar);
	key->setContext(Qt::ApplicationShortcut);
	connect(key, SIGNAL(activated()), m_searchBar, SLOT(setFocus()));

	m_statusBtn = new QPushButton(Icon("im-user-online"),
									tr("Status"),
									this);

	m_statusBtn->setMenu(statusMenu);
	key = new Shortcut("contactListGlobalStatus", m_statusBtn);
	connect(key,SIGNAL(activated()), m_statusBtn, SLOT(showMenu()));
	m_statusBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_searchBar->setPlaceholderText(tr("Search contact"));
	m_searchBar->setContentsMargins(0, 3, 0, 3);

	layout->addWidget(m_searchBar);
	layout->addWidget(m_statusBtn);
	connect(m_searchBar, SIGNAL(textChanged(QString)), m_model, SLOT(filterList(QString)));

	foreach(Protocol *proto, Protocol::all()) {
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach(Account *account, proto->accounts()) {
			onAccountCreated(account);
		}
	}
	statusMenu->addAction(createGlobalStatusAction(Status::Online));
	statusMenu->addAction(createGlobalStatusAction(Status::FreeChat));
	statusMenu->addAction(createGlobalStatusAction(Status::Away));
	statusMenu->addAction(createGlobalStatusAction(Status::DND));
	statusMenu->addAction(createGlobalStatusAction(Status::NA));
	statusMenu->addAction(createGlobalStatusAction(Status::Invisible));
	statusMenu->addAction(createGlobalStatusAction(Status::Offline));

	Status status = Status(Status::Offline);
	m_statusBtn->setText(status.name());
	m_statusBtn->setProperty("lastStatus",qVariantFromValue(status));

	statusMenu->addSeparator();

	m_status_action = statusMenu->addAction(Icon("im-status-message-edit"),
											 tr("Set Status Text"),
											 this,
											 SLOT(showStatusDialog())
											 );

	QString last_status = Config().group("contactList").value("lastStatus",QString());
	m_statusBtn->setToolTip(last_status);
	m_status_action->setData(last_status);

	statusMenu->addSeparator();

	QTimer::singleShot(0, this, SLOT(init()));
}

void SevenWidget::save()
{
	Config config;
	config.beginGroup("contactList");
	config.setValue("geometry", saveGeometry());
	config.sync();
}

SevenWidget::~SevenWidget()
{
	save();
}

void SevenWidget::addButton(ActionGenerator *generator)
{
	//nice hack
	m_controller->addAction(generator);
	QList<QAction*> a = m_controller->menu()->actions();
	m_toolFrameWindow->addAction(a.at(0));
}

void SevenWidget::removeButton(ActionGenerator *generator)
{
	m_controller->removeAction(generator);
}

void SevenWidget::loadGeometry()
{
	QByteArray geom = Config().group("contactList").value("geometry", QByteArray());
	if (geom.isNull()) {
		QRect rect = QApplication::desktop()->availableGeometry(QCursor::pos());
		//black magic
		int width = size().width();
		int x = rect.width() - width;
		int y = 0;
		int height = rect.height();
#ifdef Q_WS_WIN
		//for stupid windows
		x -= 15;
		y += 35;
		height -= 55;
#endif
		QRect geometry(x,
						y,
						width,
						height
						);
		setGeometry(geometry);
	} else {
		restoreGeometry(geom);
	}
}

QAction *SevenWidget::createGlobalStatusAction(Status::Type type)
{
	Status s = Status(type);
	QAction *act = new QAction(s.icon(), s.name(), m_statusBtn);
	connect(act, SIGNAL(triggered(bool)), SLOT(onStatusChanged()));
	act->setParent(m_statusBtn);
	act->setData(type);
	m_statusActions.append(act);
	return act;
}

void SevenWidget::onSearchButtonToggled(bool toggled)
{
	m_searchBar->setVisible(toggled);
	if (toggled) {
		m_searchBar->setFocus(Qt::PopupFocusReason);
	}
	else
		m_searchBar->clear();
}


void SevenWidget::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	//TODO add account icon
	QAction *action = new QAction(account->status().icon(), account->id(), m_statusBtn);
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
	connect(account, SIGNAL(destroyed(QObject*)),SLOT(onAccountDestroyed(QObject*)));
	m_actions.insert(account, action);
	action->setMenu(account->menu());
	m_statusBtn->menu()->addAction(action);
}

void SevenWidget::onAccountStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Account *account = sender_cast<Account *>(sender());
	QAction *action = m_actions.value(account);
	Q_ASSERT(action);
	action->setIcon(status.icon());

	if (isStatusChange(status)) {
		m_statusBtn->setProperty("lastStatus",qVariantFromValue(status));
		m_statusBtn->setText(status.name());
	}
}

void SevenWidget::onAccountDestroyed(QObject *obj)
{
	Account *account = reinterpret_cast<Account*>(obj);
	m_actions.take(account)->deleteLater();
}

void SevenWidget::onStatusChanged()
{
	if (QAction *a = sender_cast<QAction *>(sender())) {
		Status::Type type = static_cast<Status::Type>(a->data().value<int>());
		m_statusBtn->setText(Status(type).name());
		QString text = m_status_action->data().toString();
		foreach(Account *account,Account::all()) {
			Status status = account->status();
			status.setType(type);
			status.setText(text);
			status.setProperty("changeReason",Status::ByUser);
			status.setSubtype(0);
			account->setStatus(status);
		}
	}
}

void SevenWidget::showStatusDialog()
{
	QString text = m_status_action->data().toString();
	SimpleStatusDialog *dialog = new SimpleStatusDialog(text, this);
	connect(dialog, SIGNAL(accepted()), SLOT(changeStatusTextAccepted()));
	centerizeWidget(dialog);
	SystemIntegration::show(dialog);
}

void SevenWidget::changeStatusTextAccepted()
{
	SimpleStatusDialog *dialog = qobject_cast<SimpleStatusDialog *>(sender());
	Q_ASSERT(dialog);
	QString text = dialog->statusText();
	m_status_action->setData(text);
	m_statusBtn->setToolTip(text);
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

void SevenWidget::orientationChanged()
{
}

bool SevenWidget::event(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		foreach (QAction *action,m_statusActions) {
			Status last = m_statusBtn->property("lastStatus").value<Status>();
			m_statusBtn->setText(last.name());
			Status::Type type = static_cast<Status::Type>(action->data().toInt());
			action->setText(Status(type).name());
		}
		m_status_action->setText(tr("Set Status Text"));
		event->accept();
	}
	return QWidget::event(event);
}

void SevenWidget::init()
{
	MenuController *controller = ServiceManager::getByName<MenuController*>("ContactList");
	m_toolFrameWindow->setMenu(controller->menu());
	m_toolFrameWindow->addSpace(6);
	SystemIntegration::show(m_toolFrameWindow);
}

AbstractContactModel *SevenWidget::model() const
{
	return m_model;
}


} // namespace SimpleContactList
} // namespace Core



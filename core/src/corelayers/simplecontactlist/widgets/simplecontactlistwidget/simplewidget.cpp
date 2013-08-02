/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#include "simplewidget.h"
#include <QPushButton>
#include <QAction>
#include <qutim/qtwin.h>
#include <QAbstractItemDelegate>
#include <QHBoxLayout>
#include <qutim/icon.h>
#include <qutim/config.h>
#include <qutim/utils.h>
#include <qutim/actiontoolbar.h>
#include <qutim/account.h>
#include <qutim/contact.h>
#include <qutim/systemintegration.h>
#include <qutim/simplecontactlist/simplestatusdialog.h>
#include <qutim/simplecontactlist/lineedit.h>
#include <qutim/protocol.h>
#include <qutim/shortcut.h>
#include <qutim/statusactiongenerator.h>
#include <QApplication>
#include <QLineEdit>
#include <qutim/metacontact.h>
#include <QDesktopWidget>
#include <qutim/actiongenerator.h>
#include <QTimer>
#include <QKeyEvent>

namespace Core {
namespace SimpleContactList {

static bool isStatusChange(const qutim_sdk_0_3::Status &status)
{
	if (status.type() == Status::Offline) {
		foreach(Protocol *proto, Protocol::all()) {
			foreach(Account *a, proto->accounts()) {
				//				qDebug() << a->status().name() << a->status().type();
				if (a->status().type()!=Status::Offline)
					return false;
			}
		}
		return true;
	}
	else
		return true;
}

SimpleWidget::SimpleWidget() : m_model("ContactModel")
{
	if (1) {} else Q_UNUSED(QT_TRANSLATE_NOOP("ContactList", "Default style"));
	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(deleteLater()));
	connect(ServiceManager::instance(), SIGNAL(serviceChanged(QByteArray,QObject*,QObject*)),
			this, SLOT(onServiceChanged(QByteArray,QObject*,QObject*)));
	setWindowIcon(Icon("qutim"));

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

#if defined(Q_WS_MAEMO_5)
	//Works!
	int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
#else
	int size = 16;
#endif
	QSize toolbar_size (size,size);

	m_mainToolBar = new ActionToolBar(this);
	m_mainToolBar->setWindowTitle(tr("Main Toolbar"));
	addToolBar(Qt::TopToolBarArea, m_mainToolBar);

	m_mainToolBar->setIconSize(toolbar_size);
	m_mainToolBar->setFloatable(false);
	m_mainToolBar->setMovable(false);
	m_mainToolBar->setMoveHookEnabled(true);
	m_mainToolBar->setObjectName(QLatin1String("contactListBar"));

#ifdef Q_WS_WIN
	m_mainToolBar->setStyleSheet("QToolBar{background:none;border:none;}"); //HACK
#endif

	m_searchBar = new LineEdit(this);
	m_searchBar->setVisible(false);
	layout->addWidget(m_searchBar);
	connect(m_searchBar, SIGNAL(textChanged(QString)), m_model, SLOT(setFilterFixedString(QString)));
	connect(m_searchBar, SIGNAL(textChanged(QString)), SLOT(onTextChanged(QString)));
#if (QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))
	m_searchBar->setPlaceholderText(tr("Search contact"));
#endif

	m_view = new TreeView(m_model, this);
	layout->addWidget(m_view);
	m_view->setItemDelegate(ServiceManager::getByName<QAbstractItemDelegate*>("ContactDelegate"));
	m_view->installEventFilter(this);

	QMenu *statusMenu = new QMenu(this);

	// make shortcuts
	Shortcut *key = new Shortcut("find", this);
	connect(key, SIGNAL(activated()), SLOT(onSearchActivated()));


	m_statusBtn = new QPushButton(Icon("im-user-online"),
								  tr("Status"),
								  this);
	m_statusBtn->setMenu(statusMenu);
	key = new Shortcut("contactListGlobalStatus", m_statusBtn);
	connect(key,SIGNAL(activated()), m_statusBtn, SLOT(showMenu()));
	m_statusBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	layout->addWidget(m_statusBtn);

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

#ifdef Q_WS_MAEMO_5
	m_statusBtn->setMaximumHeight(50);
	m_searchBar->setMaximumHeight(50);
	setAttribute(Qt::WA_Maemo5StackedWindow);
	Config config = Config().group(QLatin1String("Maemo5"));
	switch (config.value(QLatin1String("orientation"),0))
	{
	case 0:
		setAttribute(Qt::WA_Maemo5AutoOrientation, true);
		break;
	case 1:
		setAttribute(Qt::WA_Maemo5PortraitOrientation, true);
		break;
	case 2:
		setAttribute(Qt::WA_Maemo5LandscapeOrientation, true);
		break;
	}
	statusMenu->setStyleSheet("QMenu { padding:0px;} QMenu::item { padding:2px; } QMenu::item:selected { background-color: #00a0f8; }");
	connect(QApplication::desktop(), SIGNAL(resized(int)),
			this, SLOT(orientationChanged()));
	orientationChanged();
#endif

	QTimer::singleShot(0, this, SLOT(init()));
}

SimpleWidget::~SimpleWidget()
{
	Config config;
	config.beginGroup("contactList");
	config.setValue("geometry", saveGeometry());
	config.endGroup();
	config.sync();
}

void SimpleWidget::addButton(ActionGenerator *generator)
{
	m_mainToolBar->addAction(generator);
}

void SimpleWidget::removeButton(ActionGenerator *generator)
{
	m_mainToolBar->removeAction(generator);
}

TreeView *SimpleWidget::contactView()
{
	return m_view;
}

void SimpleWidget::loadGeometry()
{
    QByteArray geom = Config().group("contactList").value("geometry", QByteArray());
    if (!geom.isNull())
        restoreGeometry(geom);
    else {
        resize(200, 600);
    }
}

QAction *SimpleWidget::createGlobalStatusAction(Status::Type type)
{
	ActionGenerator *generator = new StatusActionGenerator(Status(type));
	QAction *action = generator->generate<QAction>();
	connect(action, SIGNAL(triggered(bool)), SLOT(onStatusChanged()));
	action->setParent(m_statusBtn);
	action->setData(type);
	m_actionGenerators << generator;
	m_statusActions.append(action);
	return action;
}

void SimpleWidget::onSearchActivated()
{
	m_searchBar->setVisible(true);
	m_searchBar->setFocus(Qt::PopupFocusReason);
}

void SimpleWidget::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	//TODO add account icon
	QAction *action = new QAction(account->status().icon(), account->id(), m_statusBtn);
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
	connect(account, SIGNAL(destroyed(QObject*)),SLOT(onAccountDestroyed(QObject*)));
	m_actions.insert(account, action);
	QMenu *contextMenu = account->menu(false);
	connect(action, SIGNAL(destroyed()), contextMenu, SLOT(deleteLater()));
	action->setMenu(contextMenu);
	m_statusBtn->menu()->addAction(action);
}

void SimpleWidget::onAccountStatusChanged(const qutim_sdk_0_3::Status &status)
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

void SimpleWidget::onAccountDestroyed(QObject *obj)
{
	Account *account = reinterpret_cast<Account*>(obj);
	m_actions.take(account)->deleteLater();
}

void SimpleWidget::onStatusChanged()
{
	if (QAction *a = sender_cast<QAction *>(sender())) {
		Status::Type type = static_cast<Status::Type>(a->data().value<int>());
		m_statusBtn->setText(Status(type).name());
		QString text = m_status_action->data().toString();
		foreach(Account *account, Account::all()) {
			Status status = account->status();
			status.setType(type);
			status.setText(text);
			status.setChangeReason(Status::ByUser);
			status.setSubtype(0);
			account->setStatus(status);
		}
	}
}

void SimpleWidget::showStatusDialog()
{
	QString text = m_status_action->data().toString();
	SimpleStatusDialog *dialog = new SimpleStatusDialog(text, this);
	connect(dialog, SIGNAL(accepted()), SLOT(changeStatusTextAccepted()));
	centerizeWidget(dialog);
	SystemIntegration::show(dialog);
}

void SimpleWidget::changeStatusTextAccepted()
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

#ifdef Q_WS_MAEMO_5
void SimpleWidget::orientationChanged()
{
	QRect screenGeometry = QApplication::desktop()->screenGeometry();
	if (screenGeometry.width() > screenGeometry.height()) {
		addToolBar(Qt::LeftToolBarArea,m_mainToolBar);
		m_mainToolBar->setOrientation(Qt::Vertical);
	}
	else {
		addToolBar(Qt::TopToolBarArea,m_mainToolBar);
		m_mainToolBar->setOrientation(Qt::Horizontal);

	}
}
#endif

bool SimpleWidget::event(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		m_status_action->setText(tr("Set Status Text"));
		event->accept();
	}
	return QMainWindow::event(event);
}

void SimpleWidget::init()
{
	MenuController *controller = ServiceManager::getByName<MenuController*>("ContactList");
	ActionGenerator *gen = new MenuActionGenerator(Icon("show-menu"), QByteArray(), controller);
	gen->setShortcut(QLatin1String("contactListActivateMainMenu"));
	QAction *before = m_mainToolBar->actions().count() ? m_mainToolBar->actions().first() : 0;
	m_mainToolBar->insertAction(before, gen);
	Config config("appearance");
	config.beginGroup("contactList");
	if(config.value<bool>("showContactListOnStartup", true))
		SystemIntegration::show(this);
}

QAbstractItemModel *SimpleWidget::model() const
{
	return m_model;
}

void SimpleWidget::onTextChanged(const QString &text)
{
	if (text.isEmpty())
		m_searchBar->hide();
}

bool SimpleWidget::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj == m_view) {
		if (ev->type() == QEvent::KeyPress) {
			QKeyEvent *event = static_cast<QKeyEvent*>(ev);
			if (m_view->hasFocus() && m_searchBar->isHidden())
				m_pressedKeys.append(event->text());

			if (m_pressedKeys.count() > 1) {
				onSearchActivated();
				m_searchBar->setText(m_pressedKeys);
				m_pressedKeys.clear();
			}
			ev->accept();
		} else if (ev->type() == QEvent::FocusOut && m_searchBar->isHidden()) {
			m_pressedKeys.clear();
		}
	}
	return QMainWindow::eventFilter(obj, ev);
}

void SimpleWidget::onServiceChanged(const QByteArray &name, QObject *now, QObject *)
{
	if (name == "ContactModel") {
		m_view->setContactModel(m_model);
		connect(m_searchBar, SIGNAL(textChanged(QString)), m_model, SLOT(setFilterFixedString(QString)));
	} else if (name == "ContactDelegate") {
		m_view->setItemDelegate(sender_cast<QAbstractItemDelegate*>(now));
	}
}

} // namespace SimpleContactList
} // namespace Core


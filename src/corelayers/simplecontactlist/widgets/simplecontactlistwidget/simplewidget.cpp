#include "simplewidget.h"
#include <QPushButton>
#include <QAction>
#include <qutim/qtwin.h>
#include <qutim/simplecontactlist/abstractcontactmodel.h>
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
#include <qutim/simplecontactlist/simplestatusdialog.h>
#include <qutim/protocol.h>
#include <qutim/shortcut.h>
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

SimpleWidget::SimpleWidget()
{
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

#if defined(Q_WS_MAEMO_5)
	//int size = 48; //TODO use relative sizes table
	//smith, please test it!
	int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
#else
	int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
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

	m_model = ServiceManager::getByName<AbstractContactModel*>("ContactModel");
	Q_ASSERT(m_model);
	m_view = new TreeView(m_model, this);
	layout->addWidget(m_view);
	m_view->setItemDelegate(ServiceManager::getByName<QAbstractItemDelegate*>("ContactDelegate"));
	m_view->installEventFilter(this);

	QHBoxLayout *bottom_layout = new QHBoxLayout(w);

	QMenu *statusMenu = new QMenu(this);

	m_searchBtn = new QPushButton(this);
	m_searchBtn->setIcon(Icon("edit-find"));
	m_searchBtn->setCheckable(true);

	// make shortcuts
	Shortcut *key = new Shortcut("find", m_searchBtn);
	key->setContext(Qt::ApplicationShortcut);
	connect(key, SIGNAL(activated()), m_searchBtn, SLOT(toggle()));
	m_searchBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

	m_searchBar = new QLineEdit(this);

	m_statusBtn = new QPushButton(Icon("im-user-online"),
								   tr("Status"),
								   this);
	m_statusBtn->setMenu(statusMenu);
	key = new Shortcut("contactListGlobalStatus", m_statusBtn);
	connect(key,SIGNAL(activated()), m_statusBtn, SLOT(showMenu()));
	m_statusBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	m_searchBar->setVisible(false);

	#if (QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))
	m_searchBar->setPlaceholderText(tr("Search contact"));
	#endif

	connect(m_searchBtn, SIGNAL(toggled(bool)), SLOT(onSearchButtonToggled(bool)));
	bottom_layout->addWidget(m_statusBtn);

	layout->addWidget(m_searchBar);
	bottom_layout->addWidget(m_searchBtn);
	bottom_layout->setSpacing(0);
	bottom_layout->setMargin(0);;
	layout->addLayout(bottom_layout);

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

#ifdef Q_WS_MAEMO_5
	m_statusBtn->setMaximumHeight(50);
	m_searchBtn->setMaximumHeight(50);
	m_widget->setAttribute(Qt::WA_Maemo5StackedWindow);
	m_widget->setAttribute(Qt::WA_Maemo5AutoOrientation, true);
	statusMenu->setStyleSheet("QMenu { padding:0px;} QMenu::item { padding:2px; } QMenu::item:selected { background-color: #00a0f8; }");
#endif

	//TODO, Smith, separate it to another plugin!
#ifdef QUTIM_MOBILE_UI
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

void SimpleWidget::removeButton(ActionGenerator *)
{
	//m_mainToolBar->removeAction(generator);
}

TreeView *SimpleWidget::contactView()
{
	return m_view;
}

void SimpleWidget::loadGeometry()
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

QAction *SimpleWidget::createGlobalStatusAction(Status::Type type)
{
	Status s = Status(type);
	QAction *act = new QAction(s.icon(), s.name(), m_statusBtn);
	connect(act, SIGNAL(triggered(bool)), SLOT(onStatusChanged()));
	act->setParent(m_statusBtn);
	act->setData(type);
	m_statusActions.append(act);
	return act;
}

void SimpleWidget::onSearchButtonToggled(bool toggled)
{
	m_searchBar->setVisible(toggled);
	if (toggled) {
		m_searchBar->setFocus(Qt::PopupFocusReason);
	}
	else
		m_searchBar->clear();
}


void SimpleWidget::onAccountCreated(qutim_sdk_0_3::Account *account)
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

bool SimpleWidget::event(QEvent *event)
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
	return QMainWindow::event(event);
}

void SimpleWidget::init()
{
	MenuController *controller = ServiceManager::getByName<MenuController*>("ContactList");
	ActionGenerator *gen = new MenuActionGenerator(Icon("show-menu"), QByteArray(), controller);
	gen->setShortcut(Shortcut::getSequence("contactListActivateMainMenu").key);
	QAction *before = m_mainToolBar->actions().count() ? m_mainToolBar->actions().first() : 0;
	m_mainToolBar->insertAction(before, gen);
	SystemIntegration::show(this);
}

AbstractContactModel *SimpleWidget::model() const
{
	return m_model;
}

bool SimpleWidget::eventFilter(QObject *obj, QEvent *ev)
{
	if (obj == m_view) {
		if (ev->type() == QEvent::KeyPress) {
			QKeyEvent *event = static_cast<QKeyEvent*>(ev);
			if (m_view->hasFocus() && m_searchBar->isHidden())
				m_pressedKeys.append(event->text());

			if (m_pressedKeys.count() > 2) {
				m_searchBtn->setChecked(true);
				m_searchBar->setText(m_pressedKeys);
				m_pressedKeys.clear();
			}
			ev->accept();
		} else if (ev->type() == QEvent::FocusOut && m_searchBar->isHidden()) {
			m_pressedKeys.clear();
			m_searchBtn->setChecked(false);
		}
	}
	QMainWindow::eventFilter(obj, ev);
}

} // namespace SimpleContactList
} // namespace Core


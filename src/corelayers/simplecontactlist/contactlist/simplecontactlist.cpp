#include "simplecontactlist.h"
#include "simplecontactlistview.h"
#include "simplecontactlistmodel.h"
#include "simplecontactlistitem.h"
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/icon.h>
#include <QTreeView>
#include <qutim/debug.h>
#include <QStringBuilder>
#include <QVBoxLayout>
#include <QToolButton>
#include <QMenu>
#include <QCoreApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QPushButton>
#include <QLineEdit>
#include <qutim/qtwin.h>
#include <qutim/shortcut.h>
#include <qutim/metacontactmanager.h>
#include <QMainWindow>
#include "simplestatusdialog.h"
#include <QClipboard>
#include <qutim/servicemanager.h>
#include <qutim/settingslayer.h>
#include <QAbstractItemDelegate>

namespace Core
{
namespace SimpleContactList
{
class CopyIdGenerator : public ActionGenerator
{
public:
	CopyIdGenerator(QObject *obj) :
			ActionGenerator(Icon("edit-copy"),QT_TRANSLATE_NOOP("ContactList", "Copy id to clipboard"),obj,SLOT(onCopyIdTriggered(QObject*)))
	{
		setType(ActionTypeContactList|ActionTypeAdditional);
	}
};

class MyWidget : public QMainWindow
{
public:
	MyWidget()
	{
		resize(150,0);//hack
		connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(deleteLater()));
		setWindowIcon(Icon("qutim"));
		setAttribute(Qt::WA_AlwaysShowToolTips);
	}
	void loadGeometry()
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

	virtual ~MyWidget()
	{
		Config config;
		config.beginGroup("contactList");
		config.setValue("geometry", saveGeometry());
	}
};

struct ModulePrivate
{
	MyWidget *widget;
	TreeView *view;
	Model *model;
	ActionToolBar *mainToolBar;
	QPushButton *statusBtn;
	QPushButton *searchBtn;
	QLineEdit *searchBar;
	QHash<Account *, QAction *> actions;
	QAction *status_action;
	QList<QAction *> statusActions;
};

Module::Module() : p(new ModulePrivate)
{
	// init shortcuts
	Shortcut::registerSequence("contactListGlobalStatus",
							   QT_TRANSLATE_NOOP("ContactList", "Change global status"),
							   "ContactListWidget",
							   QKeySequence("Ctrl+S")
							   );
	Shortcut::registerSequence("contactListActivateMainMenu",
							   QT_TRANSLATE_NOOP("ContactList", "Activate main menu"),
							   "ContactListWidget",
							   QKeySequence("Ctrl+M")
							   );

	p->widget = new MyWidget;
	p->widget->setCentralWidget(new QWidget(p->widget));
	p->widget->setUnifiedTitleAndToolBarOnMac(true);
	p->widget->installEventFilter(this);
	QVBoxLayout *layout = new QVBoxLayout(p->widget->centralWidget());
	layout->setMargin(0);
	layout->setSpacing(0);

	if (QtWin::isCompositionEnabled()) {
		QtWin::extendFrameIntoClientArea(p->widget);
		p->widget->setContentsMargins(0, 0, 0, 0);
	}

	int size = Config().group("contactList").value("toolBarIconSize",16);

	QSize toolbar_size (size,size);

	p->mainToolBar = new ActionToolBar(p->widget);
	p->mainToolBar->setWindowTitle(tr("Main Toolbar"));
	p->widget->addToolBar(Qt::TopToolBarArea,p->mainToolBar);
	p->mainToolBar->setIconSize(toolbar_size);
	p->mainToolBar->setFloatable(false);
	p->mainToolBar->setMovable(false);
	p->mainToolBar->setMoveHookEnabled(true);

#ifdef Q_WS_WIN
	p->mainToolBar->setStyleSheet("QToolBar{background:none;border:none;}"); //HACK
#endif

	ActionGenerator *gen = new ActionGenerator(Icon("configure"),
											   QT_TRANSLATE_NOOP("ContactList", "&Settings..."),
											   this,
											   SLOT(onConfigureClicked(QObject*))
											   );
	gen->setPriority(1);
	gen->setType(ActionTypeAdditional);
	gen->setToolTip(QT_TRANSLATE_NOOP("ContactList","Main menu"));
	addAction(gen);

	gen = new ActionGenerator(Icon("application-exit"),
							  QT_TRANSLATE_NOOP("ContactList","&Quit"),
							  this,
							  SLOT(onQuitTriggered(QObject*)));
	gen->setPriority(-127);
	gen->setType(512);
	addAction(gen);

	gen = new MenuActionGenerator(Icon("show-menu"), QByteArray(), this);
	addButton(gen);

	p->view = new TreeView(p->widget);
	layout->addWidget(p->view);

	p->model = new Model(p->view);

	// TODO: choose another, non-kopete icon
	gen = new ActionGenerator(Icon("view-user-offline-kopete"),QT_TRANSLATE_NOOP("ContactList","Show/Hide offline"), p->model, SLOT(onHideShowOffline()));
	gen->setCheckable(true);
	gen->setChecked(!p->model->showOffline());
	gen->setToolTip(QT_TRANSLATE_NOOP("ContactList","Hide offline"));
	addButton(gen);

	//TODO move to another class
	gen = new CopyIdGenerator(this);
	gen->setPriority(-100);
	gen->addHandler(ActionVisibilityChangedHandler,this);
	MenuController::addAction<ChatUnit>(gen);

	p->view->setItemDelegate(ServiceManager::getByName<QAbstractItemDelegate*>("ContactDelegate"));
	p->view->setModel(p->model);

	QHBoxLayout *bottom_layout = new QHBoxLayout(p->widget->centralWidget());

	p->statusBtn = new QPushButton(Icon("im-user-online"),
								   tr("Status"),
								   p->widget);
	p->statusBtn->setMenu(new QMenu(p->widget));

	p->searchBtn = new QPushButton(p->widget);
	p->searchBtn->setIcon(Icon("edit-find"));
	p->searchBtn->setCheckable(true);

	// make shortcuts
	Shortcut *key = new Shortcut("find",p->searchBtn);
	key->setContext(Qt::ApplicationShortcut);
	connect(key,SIGNAL(activated()),p->searchBtn,SLOT(toggle()));
	key = new Shortcut("contactListGlobalStatus",p->statusBtn);
	connect(key,SIGNAL(activated()),p->statusBtn,SLOT(showMenu()));

	p->statusBtn->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
	p->searchBtn->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);

	p->searchBar = new QLineEdit(p->widget);
	p->searchBar->setVisible(false);
	connect(p->searchBtn,SIGNAL(toggled(bool)),SLOT(onSearchButtonToggled(bool)));
	connect(p->searchBar, SIGNAL(textChanged(QString)), p->model, SLOT(onFilterList(QString)));

	layout->addWidget(p->searchBar);
	bottom_layout->addWidget(p->statusBtn);
	bottom_layout->addWidget(p->searchBtn);
	bottom_layout->setSpacing(0);
	bottom_layout->setMargin(0);;
	layout->addLayout(bottom_layout);

	connect(MetaContactManager::instance(), SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			this, SLOT(addContact(qutim_sdk_0_3::Contact*)));

	foreach(Protocol *proto, Protocol::all()) {
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach(Account *account, proto->accounts()) {
			onAccountCreated(account);
		}
	}
	p->statusBtn->menu()->addAction(createGlobalStatusAction(Status::Online));
	p->statusBtn->menu()->addAction(createGlobalStatusAction(Status::FreeChat));
	p->statusBtn->menu()->addAction(createGlobalStatusAction(Status::Away));
	p->statusBtn->menu()->addAction(createGlobalStatusAction(Status::DND));
	p->statusBtn->menu()->addAction(createGlobalStatusAction(Status::NA));
	p->statusBtn->menu()->addAction(createGlobalStatusAction(Status::Invisible));
	p->statusBtn->menu()->addAction(createGlobalStatusAction(Status::Offline));

	Status status = Status(Status::Offline);
	p->statusBtn->setText(status.name());
	p->statusBtn->setProperty("lastStatus",qVariantFromValue(status));

	p->statusBtn->menu()->addSeparator();

	p->status_action = p->statusBtn->menu()->addAction(Icon("im-status-message-edit"),
													   tr("Set Status Text"),
													   this,
													   SLOT(showStatusDialog())
													   );

	QString last_status = Config().group("contactList").value("lastStatus",QString());
	p->statusBtn->setToolTip(last_status);
	p->status_action->setData(last_status);

	p->statusBtn->menu()->addSeparator();

	p->widget->loadGeometry();
	p->widget->show();
}

void Module::onStatusChanged()
{
	if (QAction *a = qobject_cast<QAction *>(sender())) {
		Status::Type type = static_cast<Status::Type>(a->data().value<int>());
		p->statusBtn->setText(Status(type).name());
		QString text = p->status_action->data().toString();
		foreach(Protocol *proto, Protocol::all()) {
			foreach(Account *account, proto->accounts()) {
				Status status = account->status();
				status.setType(type);
				status.setText(text);
				status.setSubtype(0);
				account->setStatus(status);
			}
		}
	}
}

Module::~Module()
{
}

QWidget *Module::widget()
{
	return p->widget;
}

void Module::addContact(qutim_sdk_0_3::Contact *contact)
{
	p->model->addContact(contact);
}

void Module::addButton(ActionGenerator *generator)
{
	p->mainToolBar->addAction(generator);
}

void Module::show()
{
	p->widget->show();
	p->widget->setWindowState(p->widget->windowState() & ~Qt::WindowMinimized);
	p->widget->activateWindow();
	p->widget->raise();
}

void Module::hide()
{
	p->widget->hide();
}

void Module::changeVisibility()
{
	//			if (p->widget->isVisible() && !p->widget->isMinimized()) {
	if (p->widget->isActiveWindow()) {
		QTimer::singleShot( 0, p->widget, SLOT(hide()) );
	} else {
		p->widget->show();
		p->widget->setWindowState(p->widget->windowState() & ~Qt::WindowMinimized);
		p->widget->activateWindow();
		p->widget->raise();
	}

}

void Module::onConfigureClicked(QObject*)
{
	Settings::showWidget();
}

void Module::onAccountCreated(Account *account)
{
	//TODO add account icon
	QAction *action = new QAction(account->status().icon(), account->id(), p->statusBtn);
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
	connect(account, SIGNAL(destroyed(QObject*)),SLOT(onAccountDestroyed(QObject*)));
	p->actions.insert(account, action);
	//			connect(action, SIGNAL(triggered()), action, SLOT(toggle()));
	action->setMenu(account->menu());
	p->statusBtn->menu()->addAction(action);
	foreach (Contact *contact, account->findChildren<Contact *>()) {
		//FIXME
		addContact(contact);
	}
	connect(account,SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),SLOT(addContact(qutim_sdk_0_3::Contact*)));
}

inline bool isStatusChange(const qutim_sdk_0_3::Status &status)
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

void Module::onAccountStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Account *account = qobject_cast<Account *>(sender());
	Q_ASSERT(account);
	QAction *action = p->actions.value(account);
	Q_ASSERT(action);
	action->setIcon(status.icon());

	if (isStatusChange(status)) {
		p->statusBtn->setProperty("lastStatus",qVariantFromValue(status));
		p->statusBtn->setText(status.name());
	}
}

QAction *Module::createGlobalStatusAction(Status::Type type)
{
	Status s = Status(type);
	QAction *act = new QAction(s.icon(),s.name(),p->statusBtn);
	connect(act,SIGNAL(triggered(bool)),SLOT(onStatusChanged()));
	act->setParent(p->statusBtn);
	act->setData(type);
	p->statusActions.append(act);
	return act;
}

void Module::onSearchButtonToggled(bool toggled)
{
	p->searchBar->setVisible(toggled);
	if (toggled) {
		p->searchBar->setFocus(Qt::PopupFocusReason);
	}
	else
		p->searchBar->clear();
}

void Module::showStatusDialog()
{
	QString text = p->status_action->data().toString();
	SimpleStatusDialog *dialog = new SimpleStatusDialog(text,p->widget);
	connect(dialog,SIGNAL(accepted()),SLOT(changeStatusTextAccepted()));
	centerizeWidget(dialog);
	dialog->show();
}

void Module::changeStatusTextAccepted()
{
	SimpleStatusDialog *dialog = qobject_cast<SimpleStatusDialog *>(sender());
	Q_ASSERT(dialog);
	QString text = dialog->statusText();
	p->status_action->setData(text);
	p->statusBtn->setToolTip(text);
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

void Module::onCopyIdTriggered(QObject *obj)
{
	ChatUnit *unit = qobject_cast<ChatUnit*>(obj);
	Q_ASSERT(unit);
	QClipboard *clipboard = QApplication::clipboard();
	clipboard->setText(unit->id());
}

void Module::onQuitTriggered(QObject *)
{
	qApp->quit();
}

bool Module::event(QEvent *ev)
{
	if (ev->type() == ActionVisibilityChangedEvent::eventType()) {
		ActionVisibilityChangedEvent *event = static_cast<ActionVisibilityChangedEvent*>(ev);
		if (event->isVisible()) {
			QAction *action = event->action();
			ChatUnit *unit = qobject_cast<ChatUnit*>(event->controller());
			Q_ASSERT(unit);
			QString id =  unit->account()->protocol()->data(Protocol::ProtocolIdName).toString();
			action->setText(QObject::tr("Copy %1 to clipboard").arg(id));
		}
	}
	return QObject::event(ev);
}

void Module::onAccountDestroyed(QObject *obj)
{
	Account *account = reinterpret_cast<Account*>(obj);
	p->actions.take(account)->deleteLater();
}

bool Module::eventFilter(QObject *obj, QEvent *event)
{
	if (obj->metaObject() == &QMainWindow::staticMetaObject) {
		if (event->type() == QEvent::LanguageChange) {
			foreach (QAction *action,p->statusActions) {
				Status last = p->statusBtn->property("lastStatus").value<Status>();
				p->statusBtn->setText(last.name());
				Status::Type type = static_cast<Status::Type>(action->data().toInt());
				action->setText(Status(type).name());
			}
			p->status_action->setText(tr("Set Status Text"));
			event->accept();
		}
	}
	return MenuController::eventFilter(obj,event);
}

}
}

#include "simplecontactlist.h"
#include "simplecontactlistview.h"
#include "simplecontactlistitem.h"
#include "abstractcontactmodel.h"
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
#include <QTimer>
#include <QPushButton>
#include <QLineEdit>
#include <qutim/qtwin.h>
#include <qutim/shortcut.h>
#include <qutim/metacontactmanager.h>
#include <QMainWindow>
#include "simplestatusdialog.h"
#include <qutim/servicemanager.h>
#include <qutim/settingslayer.h>
#include <QAbstractItemDelegate>
#include <qutim/servicemanager.h>
#include <qutim/systemintegration.h>
#include <QMenuBar>
#include <QApplication>
#include <QDesktopWidget>
#include <QWidgetAction>
#include <qutim/utils.h>

namespace Core
{
namespace SimpleContactList
{

struct ModulePrivate
{
	~ModulePrivate() {}
	QWidget *widget;
	AbstractContactListWidget *contactListWidget;
	AbstractContactModel *model;
	QScopedPointer<ActionGenerator> tagsGenerator;
};

Module::Module() : p(new ModulePrivate)
{
	Q_UNUSED(QT_TRANSLATE_NOOP("Service", "ContactList"));
	Q_UNUSED(QT_TRANSLATE_NOOP("Service", "ContactListWidget"));
	Q_UNUSED(QT_TRANSLATE_NOOP("Service", "ContactModel"));
	Q_UNUSED(QT_TRANSLATE_NOOP("Service", "ContactDelegate"));

	// init shortcuts
	Shortcut::registerSequence("contactListGlobalStatus",
							   QT_TRANSLATE_NOOP("ContactList", "Change global status"),
							   QT_TRANSLATE_NOOP("ChatLayer", "ContactList"),
							   QKeySequence("Ctrl+S")
							   );
	Shortcut::registerSequence("contactListActivateMainMenu",
							   QT_TRANSLATE_NOOP("ContactList", "Activate main menu"),
							   QT_TRANSLATE_NOOP("ChatLayer", "ContactList"),
							   QKeySequence("Ctrl+M")
							   );

	QObject *object = ServiceManager::getByName("ContactListWidget");
	p->widget = sender_cast<QWidget*>(object);
	p->contactListWidget = sender_cast<AbstractContactListWidget*>(object);
	p->model = ServiceManager::getByName<AbstractContactModel*>("ContactModel");

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

	p->tagsGenerator.reset(new ActionGenerator(Icon("feed-subscribe"), QT_TRANSLATE_NOOP("ContactList", "Select tags"), 0));
	p->tagsGenerator->addHandler(ActionCreatedHandler,this);
	p->tagsGenerator->setPriority(-127);
	addButton(p->tagsGenerator.data());

	// TODO: choose another, non-kopete icon
	gen = new ActionGenerator(Icon("view-user-offline-kopete"),QT_TRANSLATE_NOOP("ContactList","Show/Hide offline"), p->model, SLOT(hideShowOffline()));
	gen->setCheckable(true);
	gen->setChecked(!p->model->showOffline());
	gen->setToolTip(QT_TRANSLATE_NOOP("ContactList","Hide offline"));
	addButton(gen);

	//TODO move to model
	connect(MetaContactManager::instance(), SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			this, SLOT(addContact(qutim_sdk_0_3::Contact*)));
	foreach(Protocol *proto, Protocol::all()) {
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach(Account *account, proto->accounts()) {
			onAccountCreated(account);
		}
	}
	SystemIntegration::show(p->widget);
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
	p->contactListWidget->addButton(generator);
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
		SystemIntegration::show(p->widget);
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
	foreach (Contact *contact, account->findChildren<Contact *>()) {
		//FIXME move to model
		addContact(contact);
	}
	connect(account, SIGNAL(contactCreated(qutim_sdk_0_3::Contact*)),
			this, SLOT(addContact(qutim_sdk_0_3::Contact*)));
}

void Module::onQuitTriggered(QObject *)
{
	qApp->quit();
}

bool Module::event(QEvent *ev)
{
	if (ev->type() == ActionCreatedEvent::eventType()) {
		ActionCreatedEvent *event = static_cast<ActionCreatedEvent*>(ev);
		if (event->generator() == p->tagsGenerator.data()) {
			QAction *action = event->action();
			QMenu *menu = new QMenu(p->widget);
			QAction *act = menu->addAction(tr("Select tags"));
			connect(act, SIGNAL(triggered()), p->widget, SLOT(onSelectTagsTriggered()));
			act = menu->addAction(tr("Reset"));
			connect(act, SIGNAL(triggered()), p->widget, SLOT(onResetTagsTriggered()));
			action->setMenu(menu);
		}
	}
	return QObject::event(ev);
}

}
}

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
#include <QMainWindow>
#include "simplestatusdialog.h"
#include <qutim/servicemanager.h>
#include <qutim/settingslayer.h>
#include <QAbstractItemDelegate>
#include <qutim/servicemanager.h>
#include <qutim/systemintegration.h>
#include <qutim/metacontactmanager.h>
#include <QMenuBar>
#include <QApplication>
#include <QDesktopWidget>
#include <QWidgetAction>
#include <qutim/utils.h>
#include "tagsfilterdialog.h"
#include <qutim/contact.h>

namespace Core
{
namespace SimpleContactList
{

struct ModulePrivate
{
	~ModulePrivate() {}
	QWidget *widget;
	AbstractContactModel *model;
	AbstractContactListWidget *contactListWidget;
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
	p->widget = qobject_cast<QWidget*>(object);
	p->contactListWidget = qobject_cast<AbstractContactListWidget*>(object);

	ActionGenerator *gen = new ActionGenerator(Icon("configure"),
											   QT_TRANSLATE_NOOP("ContactList", "&Settings..."),
											   this,
											   SLOT(onConfigureClicked(QObject*))
											   );
	gen->setMenuRole(QAction::PreferencesRole);
	gen->setPriority(1);
	gen->setType(ActionTypeAdditional);
	gen->setToolTip(QT_TRANSLATE_NOOP("ContactList","Main menu"));	
	addAction(gen);

	gen = new ActionGenerator(Icon("application-exit"),
							  QT_TRANSLATE_NOOP("ContactList","&Quit"),
							  this,
							  SLOT(onQuitTriggered(QObject*)));
	gen->setMenuRole(QAction::QuitRole);
	gen->setPriority(-127);
	gen->setType(512);
	addAction(gen);

	object = ServiceManager::getByName("ContactModel");
	p->model = dynamic_cast<AbstractContactModel*>(object);
	qDebug() << object << p->model << qobject_cast<QAbstractItemModel*>(object);

	connect(ServiceManager::instance(), SIGNAL(serviceChanged(QByteArray,QObject*,QObject*)),
			SLOT(onServiceChanged(QByteArray,QObject*,QObject*)));
	QTimer::singleShot(0, this, SLOT(init()));
}

Module::~Module()
{
}

QWidget *Module::widget()
{
	return p->widget;
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
			connect(act, SIGNAL(triggered()), this, SLOT(onSelectTagsTriggered()));
			act = menu->addAction(tr("Reset"));
			connect(act, SIGNAL(triggered()), this, SLOT(onResetTagsTriggered()));
			action->setMenu(menu);
		}
	}
	return QObject::event(ev);
}

void Module::init()
{
	p->tagsGenerator.reset(new ActionGenerator(Icon("feed-subscribe"), QT_TRANSLATE_NOOP("ContactList", "Select tags"), 0));
	p->tagsGenerator->addHandler(ActionCreatedHandler,this);
	p->tagsGenerator->setPriority(-127);
	addButton(p->tagsGenerator.data());

	// TODO: choose another, non-kopete icon
	ActionGenerator *gen = new ActionGenerator(Icon("view-user-offline-kopete"),QT_TRANSLATE_NOOP("ContactList","Show/Hide offline"), p->model, SLOT(hideShowOffline()));
	gen->setCheckable(true);
	gen->setChecked(!p->model->showOffline());
	gen->setToolTip(QT_TRANSLATE_NOOP("ContactList","Hide offline"));
	addButton(gen);
}

void Module::onResetTagsTriggered()
{
	p->model->filterList(QStringList());
}

void Module::onSelectTagsTriggered()
{
	QStringList tags = p->model->tags();
	TagsFilterDialog *dialog = new TagsFilterDialog(tags, p->widget);
	if (!p->model->selectedTags().isEmpty())
		tags = p->model->selectedTags().toList();
	dialog->setSelectedTags(tags);
	SystemIntegration::show(dialog);
	centerizeWidget(dialog);
	if (dialog->exec()) {
		p->model->filterList(dialog->selectedTags());
	}
	dialog->deleteLater();
}

void Module::addContact(qutim_sdk_0_3::Contact *contact)
{
#if 1
	p->model->metaObject()->invokeMethod(p->model, "addContact",
								   Q_ARG(qutim_sdk_0_3::Contact*, contact));
#else
	p->model->addContact(contact);
#endif
}

void Module::onServiceChanged(const QByteArray &name, QObject *now, QObject *old)
{
	Q_UNUSED(old);
	if (old == p->model && name == "ContactModel") {
		QList<Contact*> contacts = p->model->contacts();
		p->model = dynamic_cast<AbstractContactModel*>(now);
		Q_ASSERT(p->model);
		TreeView *view = p->contactListWidget->contactView();
		Q_ASSERT(view);
		view->setModel(p->model);
		p->model->setContacts(contacts);
	}
}

}
}

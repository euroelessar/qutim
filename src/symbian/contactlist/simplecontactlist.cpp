#include "simplecontactlist.h"
#include "simplecontactlistview.h"
#include "simplecontactlistmodel.h"
#include "simplecontactlistitem.h"
#include "simplecontactlistdelegate.h"
#include "src/modulemanagerimpl.h"
#include "libqutim/protocol.h"
#include "libqutim/account.h"
#include "libqutim/icon.h"
#include "libqutim/settingslayer.h"
#include <QTreeView>
#include <libqutim/debug.h>
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
#include <libqutim/qtwin.h>
#include <libqutim/shortcut.h>

namespace Core
{
	namespace SimpleContactList
	{
		static CoreSingleModuleHelper<Module> contact_list_static(
				QT_TRANSLATE_NOOP("Plugin", "Mobile ContactList"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM contact list implementation for mobile devices")
				);

		class MyWidget : public QWidget
		{
		public:
			MyWidget()
			{
			}
			virtual ~MyWidget()
			{

			}
		};

		struct ModulePrivate
		{
			MyWidget *widget;
			TreeView *view;
			Model *model;
			QLineEdit *search_bar;
			QHash<Account *, QAction *> actions;
		};

		Module::Module() : p(new ModulePrivate)
		{
			p->widget = new MyWidget;
			QVBoxLayout *layout = new QVBoxLayout(p->widget);
			layout->setMargin(0);
			layout->setSpacing(0);

			ActionGenerator *gen = new ActionGenerator(Icon("configure"),
										  QT_TRANSLATE_NOOP("ContactList", "&Settings..."),
										  this,
										  SLOT(onConfigureClicked())
										  );
			gen->setPriority(1);
			gen->setToolTip(QT_TRANSLATE_NOOP("ContactList","Main menu"));
			addAction(gen);

			p->view = new TreeView(p->widget);
			layout->addWidget(p->view);

			p->widget->setLayout(layout);
			p->model = new Model(p->view);

			p->view->setItemDelegate(new SimpleContactListDelegate(p->view));
			p->view->setModel(p->model);

			p->search_bar = new QLineEdit(p->widget);
			connect(p->search_bar, SIGNAL(textChanged(QString)), p->model, SLOT(onFilterList(QString)));

			layout->addWidget(p->search_bar);

			foreach(Protocol *proto, allProtocols()) {
				connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
				foreach(Account *account, proto->accounts()) {
					onAccountCreated(account);
				}
			}
			p->widget->addAction(createGlobalStatusAction(Status::Online));
			p->widget->addAction(createGlobalStatusAction(Status::FreeChat));
			p->widget->addAction(createGlobalStatusAction(Status::Away));
			p->widget->addAction(createGlobalStatusAction(Status::DND));
			p->widget->addAction(createGlobalStatusAction(Status::NA));
			p->widget->addAction(createGlobalStatusAction(Status::Invisible));
			p->widget->addAction(createGlobalStatusAction(Status::Offline));
		}

		void Module::onStatusChanged()
		{
			if (QAction *a = qobject_cast<QAction *>(sender())) {
				Status status= a->data().value<Status>();
				foreach(Protocol *proto, allProtocols()) {
					foreach(Account *account, proto->accounts()) {
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

		void Module::addContact(ChatUnit *unit)
		{
			if(Contact *contact = qobject_cast<Contact *>(unit))
				p->model->addContact(contact);
		}

		void Module::removeContact(ChatUnit *unit)
		{
			if(Contact *contact = qobject_cast<Contact *>(unit))
				p->model->removeContact(contact);
		}

		void Module::removeAccount(Account *account)
		{
			foreach(Contact *contact, account->findChildren<Contact *>())
				removeContact(contact);
		}

		void Module::show()
		{
			p->widget->show();
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

		void Module::onConfigureClicked()
		{
			Settings::showWidget();
		}

		void Module::onAccountCreated(Account *account)
		{
			//TODO add account icon
			QAction *action = new QAction(account->status().icon(), account->id(), p->widget);
			connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status)),
					this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
			p->actions.insert(account, action);
//			connect(action, SIGNAL(triggered()), action, SLOT(toggle()));
			action->setMenu(account->menu(false));
			p->widget->addAction(action);
			foreach (Contact *contact, account->findChildren<Contact *>()) {
				//FIXME
				addContact(contact);
			}
		}

		void Module::onAccountStatusChanged(const qutim_sdk_0_3::Status &status)
		{
			Account *account = qobject_cast<Account *>(sender());
			Q_ASSERT(account);
			QAction *action = p->actions.value(account);
			Q_ASSERT(action);
			action->setIcon(status.icon());
		}

		QAction *Module::createGlobalStatusAction(Status::Type type)
		{
			Status s (type);
			QAction *act = new QAction(s.icon(),s.name(),p->widget);
			act->setData(QVariant::fromValue(s));
			connect(act,SIGNAL(triggered()),SLOT(onStatusChanged()));
			return act;
		}

		void Module::onSearchButtonToggled(bool toggled)
		{
			p->search_bar->setVisible(toggled);
			if (toggled) {
				p->search_bar->setFocus(Qt::PopupFocusReason);
			}
			else
				p->search_bar->clear();
		}
	}
}

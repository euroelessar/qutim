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
#include <QDebug>
#include <QStringBuilder>
#include <QVBoxLayout>
#include <QToolButton>
#include <QMenu>
#include <QCoreApplication>
#include <QApplication>
#include <QDesktopWidget>

namespace Core
{
	namespace SimpleContactList
	{
		static CoreSingleModuleHelper<Module> contact_list_static(
				QT_TRANSLATE_NOOP("Plugin", "Simple ContactList"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM contact list realization. Just simple")
				);

		class MyWidget : public QWidget
		{
		public:
			MyWidget()
			{
				QByteArray geom = Config().group("contactList").value("geometry", QByteArray());
				if (geom.isNull()) {
					int width = 150; //TODO: what to do? o.O
					QRect rect = QApplication::desktop()->availableGeometry(QCursor::pos());
					rect.setX(rect.width() - width);
					rect.setWidth(width);
					setGeometry(rect);
				} else
					restoreGeometry(geom);
				connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(deleteLater()));
			}

			virtual ~MyWidget()
			{
				Config config;
				config.setValue("contactList/geometry", saveGeometry());
				config.sync();
			}
		};

		struct ModulePrivate
		{
			QWidget *widget;
			TreeView *view;
			Model *model;
			ActionToolBar *main_toolbar;
			ActionToolBar *bottom_toolbar;
			QHash<Account *, QAction *> actions;
		};

		Module::Module() : p(new ModulePrivate)
		{
			p->widget = new MyWidget;
			QVBoxLayout *layout = new QVBoxLayout(p->widget);
			layout->setMargin(0);

			p->main_toolbar = new ActionToolBar(p->widget);
			layout->addWidget(p->main_toolbar);
			
			addAction(new ActionGenerator(Icon("configure"),
										  QT_TRANSLATE_NOOP("simplecontactlist","&Settings..."),
										  this,
										  SLOT(onConfigureClicked()))
										  );
			addAction(new ActionGenerator(Icon("application-exit"),
										  QT_TRANSLATE_NOOP("simplecontactlist","&Quit"),
										  qApp,
										  SLOT(quit()))
										  );
			
// 			QMenu *menu = new QMenu(tr("Main menu"), p->main_toolbar);
// 			menu->addAction(Icon("configure"), tr("&Settings..."), this, SLOT(onConfigureClicked()));
// 			menu->addAction(Icon("application-exit"), tr("&Quit"), qApp, SLOT(quit()));

			QAction *menuAction = new QAction(Icon("show-menu"), tr("Main menu"), this);
			
			menuAction->setMenu(menu());
			p->main_toolbar->addAction(menuAction);

			p->view = new TreeView(p->widget);
			layout->addWidget(p->view);

			p->widget->setLayout(layout);
			p->model = new Model(p->view);
			p->view->setItemDelegate(new SimpleContactListDelegate(p->view));
			p->view->setModel(p->model);
			p->widget->show();

			p->bottom_toolbar = new ActionToolBar(p->widget);
			layout->addWidget(p->bottom_toolbar);
			foreach(Protocol *proto, allProtocols()) {
				connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
				foreach(Account *account, proto->accounts()) {
					onAccountCreated(account);
				}
			}
		}

		Module::~Module()
		{
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

		void Module::addButton(ActionGenerator *generator)
		{
			p->main_toolbar->addAction(generator);
		}

		void Module::onConfigureClicked()
		{
			Settings::showWidget();
		}

		void Module::onAccountCreated(Account *account)
		{
			//TODO add account icon
			QAction *action = new QAction(account->status().icon(), account->name(), p->bottom_toolbar);
			connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status)),
					this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
			p->actions.insert(account, action);
//			connect(action, SIGNAL(triggered()), action, SLOT(toggle()));
			action->setMenu(account->menu(false));
			p->bottom_toolbar->addAction(action);
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
	}
}

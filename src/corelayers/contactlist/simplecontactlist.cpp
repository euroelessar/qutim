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
					rect.setHeight(rect.bottom());
					setGeometry(rect);
				} else {
					restoreGeometry(geom);
				}
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
			QPushButton *status_btn;
			QPushButton *search_btn;
			QLineEdit *search_bar;
			QHash<Account *, QAction *> actions;
		};

		Module::Module() : p(new ModulePrivate)
		{
			// init shortcuts
			Shortcut::registerSequence("contactListGlobalStatus",
									QT_TRANSLATE_NOOP("ContactList", "Change global status"),
									"ContactListWidget",
									QKeySequence("Ctrl+S")
									);
			//TODO move to separated startup module
			Shortcut::registerSequence("find",
									QT_TRANSLATE_NOOP("qutIM", "Find"),
									"ContactListWidget",
									QKeySequence(QKeySequence::Find)
									);
			Shortcut::registerSequence("contactListActivateMainMenu",
									QT_TRANSLATE_NOOP("ContactList", "Activate main menu"),
									"ContactListWidget",
									QKeySequence("Ctrl+M")
									);
			
			p->widget = new MyWidget;
			QVBoxLayout *layout = new QVBoxLayout(p->widget);
			layout->setMargin(0);
			layout->setSpacing(0);

			if (QtWin::isCompositionEnabled()) {
				QtWin::extendFrameIntoClientArea(p->widget);
				p->widget->setContentsMargins(0, 0, 0, 0);
			}
			
			int size = Config().group("contactList").value("toolBarIconSize",16);
			
			QSize toolbar_size (size,size);
			
			p->main_toolbar = new ActionToolBar(p->widget);
			p->main_toolbar->setIconSize(toolbar_size);

#ifdef Q_WS_WIN
			p->main_toolbar->setStyleSheet("QToolBar{background:none;border:none}"); //HACK
#endif
			
			layout->addWidget(p->main_toolbar);
			
			ActionGenerator *gen = new ActionGenerator(Icon("configure"),
										  QT_TRANSLATE_NOOP("ContactList", "&Settings..."),
										  this,
										  SLOT(onConfigureClicked())
										  );
			gen->setPriority(1);
			gen->setToolTip(QT_TRANSLATE_NOOP("ContactList","Main menu"));
			addAction(gen);

			gen = new ActionGenerator(Icon("application-exit"),
									  QT_TRANSLATE_NOOP("ContactList","&Quit"),
									  qApp,
									  SLOT(quit()));
			gen->setPriority(-1);
			addAction(gen);

			gen = new MenuActionGenerator(Icon("show-menu"), QByteArray(), this);
			addButton(gen);

			p->view = new TreeView(p->widget);
			layout->addWidget(p->view);

			p->widget->setLayout(layout);
			p->model = new Model(p->view);

			// TODO: choose another, non-kopete icon
			gen = new ActionGenerator(Icon("view-user-offline-kopete"), QByteArray(), p->model, SLOT(onHideShowOffline()));
			gen->setCheckable(true);
			gen->setChecked(!p->model->showOffline());
			gen->setToolTip(QT_TRANSLATE_NOOP("ContactList","Hide offline"));
			addButton(gen);

			p->view->setItemDelegate(new SimpleContactListDelegate(p->view));
			p->view->setModel(p->model);
			p->widget->show();

			QHBoxLayout *bottom_layout = new QHBoxLayout(p->widget);

			p->status_btn = new QPushButton(Icon("im-user-online"),
											  "Global status",
											  p->widget);
			p->status_btn->setMenu(new QMenu(p->widget));

			p->search_btn = new QPushButton(p->widget);
			p->search_btn->setIcon(Icon("edit-find"));
			p->search_btn->setCheckable(true);

			// make shortcuts
			Shortcut *key = new Shortcut("find",p->search_btn);
			key->setContext(Qt::ApplicationShortcut);
			connect(key,SIGNAL(activated()),p->search_btn,SLOT(toggle()));
			key = new Shortcut("contactListGlobalStatus",p->status_btn);
			connect(key,SIGNAL(activated()),p->status_btn,SLOT(showMenu()));

			p->status_btn->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
			p->search_btn->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Preferred);

			p->search_bar = new QLineEdit(p->widget);
			p->search_bar->setVisible(false);
			connect(p->search_btn,SIGNAL(toggled(bool)),SLOT(onSearchButtonToggled(bool)));
			connect(p->search_bar, SIGNAL(textChanged(QString)), p->model, SLOT(onFilterList(QString)));

			layout->addWidget(p->search_bar);
			bottom_layout->addWidget(p->status_btn);
			bottom_layout->addWidget(p->search_btn);
			bottom_layout->setSpacing(0);
			bottom_layout->setMargin(0);;
			layout->addLayout(bottom_layout);

			foreach(Protocol *proto, allProtocols()) {
				connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
				foreach(Account *account, proto->accounts()) {
					onAccountCreated(account);
				}
			}
			p->status_btn->menu()->addAction(createGlobalStatusAction(Status::Online));
			p->status_btn->menu()->addAction(createGlobalStatusAction(Status::FreeChat));
			p->status_btn->menu()->addAction(createGlobalStatusAction(Status::Away));
			p->status_btn->menu()->addAction(createGlobalStatusAction(Status::DND));
			p->status_btn->menu()->addAction(createGlobalStatusAction(Status::NA));
			p->status_btn->menu()->addAction(createGlobalStatusAction(Status::Invisible));
			p->status_btn->menu()->addAction(createGlobalStatusAction(Status::Offline));

			p->status_btn->menu()->addSeparator();

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
			QAction *action = new QAction(account->status().icon(), account->name(), p->status_btn);
			connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status)),
					this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
			p->actions.insert(account, action);
//			connect(action, SIGNAL(triggered()), action, SLOT(toggle()));
			action->setMenu(account->menu(false));
			p->status_btn->menu()->addAction(action);
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
			QAction *act = new QAction(s.icon(),s.name(),p->status_btn);
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

#include "simplecontactlist.h"
#include "simplecontactlistview.h"
#include "simplecontactlistmodel.h"
#include "simplecontactlistitem.h"
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

namespace Core
{
	namespace SimpleContactList
	{
		static CoreSingleModuleHelper<Module> contact_list_static(
				QT_TRANSLATE_NOOP("Plugin", "Simple ContactList"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM contact list realization. Just simple")
				);

		struct ModulePrivate
		{
			QWidget *widget;
			TreeView *view;
			Model *model;
		};

		Module::Module() : p(new ModulePrivate)
		{
			p->widget = new QWidget;
			QVBoxLayout *layout = new QVBoxLayout(p->widget);
			layout->setMargin(0);
			QToolButton *button = new QToolButton(p->widget);
			button->setIcon(Icon("show-menu"));
			p->view = new TreeView(p->widget);
			layout->addWidget(button);
			layout->addWidget(p->view);
			p->widget->setLayout(layout);
			p->model = new Model(p->view);
			p->view->setModel(p->model);
			p->widget->show();
			foreach(Protocol *proto, allProtocols())
				foreach(Contact *contact, proto->findChildren<Contact *>())
					addContact(contact);
			QMenu *menu = new QMenu(tr("Main menu"), button);
			menu->addAction(Icon("configure"), tr("&Settings..."), this, SLOT(onConfigureClicked()));
			menu->addAction(Icon("application-exit"), tr("&Quit"), qApp, SLOT(quit()));
			button->setMenu(menu);
			connect(button, SIGNAL(clicked()), button, SLOT(showMenu()));
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

		void Module::onConfigureClicked()
		{
			Settings::showWidget();
		}
	}
}

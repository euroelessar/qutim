#include "simplecontactlist.h"
#include "simplecontactlistview.h"
#include "simplecontactlistmodel.h"
#include "simplecontactlistitem.h"
#include "src/modulemanagerimpl.h"
#include "libqutim/protocol.h"
#include "libqutim/account.h"
#include <QTreeView>
#include <QDebug>
#include <QStringBuilder>

namespace Core
{
	namespace SimpleContactList
	{
		static CoreModuleHelper<Module> contact_list_static(
				QT_TRANSLATE_NOOP("Plugin", "Simple ContactList"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM contact list realization. Just simple")
				);

		struct ModulePrivate
		{
			TreeView *view;
			Model *model;
		};

		Module::Module() : p(new ModulePrivate)
		{
			p->view = new TreeView(0);
			p->model = new Model(p->view);
			p->view->setModel(p->model);
			p->view->show();
			foreach(Protocol *proto, allProtocols())
				foreach(Contact *contact, proto->findChildren<Contact *>())
					addContact(contact);
		}

		Module::~Module()
		{
		}

		void Module::addContact(Contact *contact)
		{
			if(p->model->containsContact(contact))
				return;
			connect(contact, SIGNAL(statusChanged(Status)),      p->model, SLOT(contactStatusChanged(Status)));
			connect(contact, SIGNAL(nameChanged(QString)),       p->model, SLOT(contactNameChanged(QString)));
			connect(contact, SIGNAL(tagsChanged(QSet<QString>)), p->model, SLOT(contactTagsChanged(QSet<QString>)));
			p->model->addContact(contact);

//			item->setToolTip(QLatin1Literal("ID: ") % contact->id() % QLatin1Literal("\n")
//							 % QLatin1Literal("Name: ") % contact->name() % QLatin1Literal("\n")
//							 % QLatin1Literal("Status: ") % statusToString(contact->status()));
		}

		void Module::removeContact(Contact *contact)
		{
			contact->disconnect(p->model);
			p->model->removeContact(contact);
		}

		void Module::removeAccount(Account *account)
		{
			foreach(Contact *contact, account->findChildren<Contact *>())
				removeContact(contact);
		}
	}
}

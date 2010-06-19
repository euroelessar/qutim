#include "addcontact.h"
#include "ui_addcontact.h"
#include "modulemanagerimpl.h"
#include <libqutim/protocol.h>
#include <libqutim/contact.h>
#include <libqutim/icon.h>
#include <libqutim/debug.h>
#include <QToolButton>

namespace Core {
	static Core::CoreModuleHelper<AddContactModule> add_contact_module_static(
		QT_TRANSLATE_NOOP("Plugin", "Add contact dialog"),
		QT_TRANSLATE_NOOP("Plugin", "Simple add contact dialog")
	);	
	
	struct AddContactPrivate
	{
		QHash<QString, Account *> accounts;
		Account *account;
		Ui::AddContact *ui;
	};

	AddContactModule::AddContactModule()
	{
		QObject *contactList = getService("ContactList");
		if (contactList) {
			static QScopedPointer<ActionGenerator> button(new ActionGenerator(Icon("list-add-user"),
					QT_TRANSLATE_NOOP("AddContact", "Add contact"), this, SLOT(show())));
			MenuController *controller = qobject_cast<MenuController*>(contactList);
			if (controller)
				controller->addAction(button.data());
		}
	}

	void AddContactModule::show()
	{
		AddContact *addContact = new AddContact();
		centerizeWidget(addContact);
		addContact->show();
	}

	AddContact::AddContact(Account *account, QWidget *parent) : QDialog(parent), d_ptr(new AddContactPrivate())
	{
		Q_D(AddContact);
		d->ui = new Ui::AddContact();
		d->ui->setupUi(this);
		if (account) {
			setAccount(account);
		} else {
			d->ui->stackedWidget->setCurrentIndex(0);
			foreach (Protocol *protocol, allProtocols())
				if (!protocol->accounts().isEmpty())
					foreach (Account *acc, protocol->accounts()) {
						d->accounts.insert(acc->id(), acc);
						QToolButton *button = new QToolButton(d->ui->accountPage);
						button->setText(acc->id());
						QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
						button->setSizePolicy(sizePolicy);
						button->setAutoRaise(true);
						connect(button, SIGNAL(clicked()), this, SLOT(setAccount()));
						d->ui->accountLayout->insertWidget(d->ui->accountLayout->count()-1, button);
					}
			if (d->accounts.count() == 1)
				setAccount(d->accounts.values().at(0));
		}

	}

	AddContact::~AddContact()
	{
	}

	void AddContact::setAccount()
	{
		if (QToolButton *button = qobject_cast<QToolButton *>(sender()))
			setAccount(d_func()->accounts.value(button->text()));
	}

	void AddContact::setAccount(Account *account)
	{
		Q_D(AddContact);
		d->account = account;
		d->ui->stackedWidget->setCurrentIndex(1);
		d->ui->IDLabel->setText(account->protocol()->data(Protocol::ProtocolIdName).toString());
		d->accounts.clear();
	}
	
	void AddContact::on_okButton_clicked()
	{
		Q_D(AddContact);
		Contact *contact = d->account->getContact(d->ui->editId->text());
		if (contact) {
			contact->setName(d->ui->editName->text());
			d->account->addContact(contact);
		}
		deleteLater();
	}

	void AddContact::on_cancelButton_clicked()
	{
		deleteLater();
	}
}

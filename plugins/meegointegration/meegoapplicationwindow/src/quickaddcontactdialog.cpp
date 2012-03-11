/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "quickaddcontactdialog.h"
#include "addcontactdialogwrapper.h"
#include <qdeclarative.h>
#include <qutim/protocol.h>
#include <qutim/contact.h>
#include <qutim/icon.h>
#include <qutim/chatsession.h>
#include <qutim/servicemanager.h>
#include <qutim/systemintegration.h>

namespace Core
{

bool isSupportAddContact()
{
	foreach (qutim_sdk_0_3::Protocol *p,qutim_sdk_0_3::Protocol::all()) {
		bool support = p->data(qutim_sdk_0_3::Protocol::ProtocolContainsContacts).toBool();
		if (support) {
			foreach (qutim_sdk_0_3::Account *a,p->accounts()) {
				if (a->status() != qutim_sdk_0_3::Status::Offline) {
					return true;
				}
			}
		}
	}
	return false;
}
}

namespace MeegoIntegration
{
QuickAddContactDialog::QuickAddContactDialog() {
	QObject *contactList = ServiceManager::getByName("ContactList");
		if (contactList) {
			m_addUserGen.reset(new ActionGenerator(Icon("list-add-user"),
												   QT_TRANSLATE_NOOP("AddContact", "Add contact"),
												   this, SLOT(show())));
			MenuController *controller = qobject_cast<MenuController*>(contactList);
			Q_ASSERT(controller);
			controller->addAction(m_addUserGen.data());
		}
}

void QuickAddContactDialog::show(Account *account, const QString &id,
			    const QString &name, const QStringList &tags)
{
//	AddContact *addContact = new AddContact(account);
//	addContact->setContactId(id);
//	addContact->setContactName(name);
//	addContact->setContactTags(tags);
//	centerizeWidget(addContact);
//	SystemIntegration::show(addContact);
}

void QuickAddContactDialog::show()
{
	AddContactDialogWrapper::showDialog(this);
}


void QuickAddContactDialog::accept(const QString &password, bool remember)
{
	//this->apply(password,remember);
}

void QuickAddContactDialog::cancel()
{
	//this->reject();
}

}


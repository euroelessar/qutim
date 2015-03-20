/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef SIMPLECONTACTLIST_H
#define SIMPLECONTACTLIST_H

#include <qutim/actiontoolbar.h>
#include <qutim/status.h>
#include <qutim/menucontroller.h>

namespace qutim_sdk_0_3
{
class Account;
class Contact;
}

using namespace qutim_sdk_0_3;

namespace Core
{
namespace SimpleContactList
{
struct ModulePrivate;

class Module : public MenuController
{
	Q_OBJECT
	Q_PROPERTY(QWidget* widget READ widget)
    Q_PROPERTY(QObject* buttons READ buttons CONSTANT)
	Q_CLASSINFO("Service", "ContactList")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "ContactListWidget")
public:
	Module();
	virtual ~Module();
	Q_INVOKABLE void addButton(qutim_sdk_0_3::ActionGenerator *generator);
	Q_INVOKABLE void removeButton(qutim_sdk_0_3::ActionGenerator *generator);
	Q_INVOKABLE QWidget *widget();
    QObject *buttons();
protected:
	bool event(QEvent *);
public slots:
	void show();
	void hide();
	void changeVisibility();
	void addContact(qutim_sdk_0_3::Contact *);
private slots:
	void onConfigureClicked(QObject*);
	void onQuitTriggered(QObject *);
	void init();
	void onResetTagsTriggered();
	void onSelectTagsTriggered();
	void onServiceChanged(const QByteArray &name, QObject *now, QObject *old);
	void onHideShowOffline();
private:
	QScopedPointer<ModulePrivate> p;
};
}
}

#endif // SIMPLECONTACTLIST_H


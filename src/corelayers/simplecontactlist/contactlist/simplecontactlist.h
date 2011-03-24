#ifndef SIMPLECONTACTLIST_H
#define SIMPLECONTACTLIST_H

#include <qutim/actiontoolbar.h>
#include <qutim/status.h>
#include <qutim/contact.h>
#include "abstractcontactlist.h"

namespace qutim_sdk_0_3
{
class Account;
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
	Q_CLASSINFO("Service", "ContactList")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "ContactListWidget")
public:
	Module();
	virtual ~Module();
	Q_INVOKABLE void addButton(ActionGenerator *generator);
	Q_INVOKABLE QWidget *widget();
protected:
	bool event(QEvent *);
public slots:
	void show();
	void hide();
	void changeVisibility();
private slots:
	void onConfigureClicked(QObject*);
	void onQuitTriggered(QObject *);
	void onAccountCreated(qutim_sdk_0_3::Account *);
	void addContact(qutim_sdk_0_3::Contact *contact);
	void init();
	void onResetTagsTriggered();
	void onSelectTagsTriggered();
private:
	QScopedPointer<ModulePrivate> p;
};
}
}

#endif // SIMPLECONTACTLIST_H

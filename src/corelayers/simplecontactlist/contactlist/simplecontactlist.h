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

class Module : public AbstractContactList
{
	Q_OBJECT
public:
	Module();
	virtual ~Module();
	virtual void addButton(ActionGenerator *generator);
	virtual QWidget *widget();
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

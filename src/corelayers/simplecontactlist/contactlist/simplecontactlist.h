#ifndef SIMPLECONTACTLIST_H
#define SIMPLECONTACTLIST_H

#include <qutim/actiontoolbar.h>
#include <qutim/status.h>
#include "abstractcontactlist.h"

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

class SIMPLECONTACTLIST_EXPORT Module : public MenuController
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
	void addContact(qutim_sdk_0_3::Contact *);
private slots:
	void onConfigureClicked(QObject*);
	void onQuitTriggered(QObject *);
	void init();
	void onResetTagsTriggered();
	void onSelectTagsTriggered();
	void onServiceChanged(const QByteArray &name, QObject *now, QObject *old);
private:
	QScopedPointer<ModulePrivate> p;
};
}
}

#endif // SIMPLECONTACTLIST_H

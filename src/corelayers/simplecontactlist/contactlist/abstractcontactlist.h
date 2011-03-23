#ifndef ABSTRACTCONTACTLIST_H
#define ABSTRACTCONTACTLIST_H
#include <qutim/menucontroller.h>
#include "simplecontactlist_global.h"

namespace Core {
namespace SimpleContactList {

using namespace qutim_sdk_0_3;

class SIMPLECONTACTLIST_EXPORT AbstractContactList : public MenuController
{
    Q_OBJECT
	Q_PROPERTY(QWidget* widget READ widget)
	Q_CLASSINFO("Service", "ContactList")
	Q_CLASSINFO("Uses", "ChatLayer")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "MetaContactManager")
	Q_CLASSINFO("Uses", "ContactDelegate")
	Q_CLASSINFO("Uses", "ContactModel")
	Q_CLASSINFO("Uses", "ContactListWidget")
public:
	explicit AbstractContactList() {};
	virtual ~AbstractContactList() {};
	Q_INVOKABLE virtual void addButton(ActionGenerator *generator) = 0;
	Q_INVOKABLE virtual QWidget *widget() = 0;
};

class SIMPLECONTACTLIST_EXPORT AbstractContactListWidget
{
public:
	virtual ~AbstractContactListWidget() {};
	virtual void addButton(ActionGenerator *generator) = 0;
	virtual void removeButton(ActionGenerator *generator) = 0;
};

} // namespace SimpleContactList
} // namespace Core

Q_DECLARE_INTERFACE(Core::SimpleContactList::AbstractContactListWidget, "org.qutim.core.AbstractContactListWidget")

#endif // ABSTRACTCONTACTLIST_H

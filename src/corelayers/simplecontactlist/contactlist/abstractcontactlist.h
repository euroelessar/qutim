#ifndef ABSTRACTCONTACTLIST_H
#define ABSTRACTCONTACTLIST_H
#include <qutim/menucontroller.h>
#include "simplecontactlist_global.h"

namespace Core {
namespace SimpleContactList {

using namespace qutim_sdk_0_3;

class AbstractContactModel;
class SIMPLECONTACTLIST_EXPORT AbstractContactListWidget
{
public:
	virtual ~AbstractContactListWidget() {}
	virtual void addButton(ActionGenerator *generator) = 0;
	virtual void removeButton(ActionGenerator *generator) = 0;
};

} // namespace SimpleContactList
} // namespace Core

Q_DECLARE_INTERFACE(Core::SimpleContactList::AbstractContactListWidget, "org.qutim.core.AbstractContactListWidget")

#endif // ABSTRACTCONTACTLIST_H

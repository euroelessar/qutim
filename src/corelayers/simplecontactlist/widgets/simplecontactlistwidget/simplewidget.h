#ifndef SIMPLEWIDGET_H
#define SIMPLEWIDGET_H
#include <QMainWindow>
#include <abstractcontactlist.h>

namespace Core {
namespace SimpleContactList {

class SimpleWidget : public QMainWindow, public AbstractContactListWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::SimpleContactList::AbstractContactListWidget)
	Q_CLASSINFO("Uses", "ContactDelegate")
	Q_CLASSINFO("Uses", "ContactModel")
	Q_CLASSINFO("Service", "ContactListWidget")
public:
	SimpleWidget();
	virtual void addButton(ActionGenerator *generator);
	virtual void removeButton(ActionGenerator *generator);
};

} // namespace SimpleContactList
} // namespace Core

#endif // SIMPLEWIDGET_H

#ifndef CORE_LASTACTIVITYCOMPARATOR_H
#define CORE_LASTACTIVITYCOMPARATOR_H
#include <qutim/contact.h>
#include "statuscomparator.h"

namespace Core {

class LastActivityComparator : public StatusComparator
{
	Q_OBJECT
	Q_CLASSINFO("SettingsDescription", "Sort by contact's last activity")
public:
	explicit LastActivityComparator();
	virtual int compare(Ureen::Contact *a, Ureen::Contact *b);
protected:
	virtual void doStartListen(Ureen::Contact *contact);
};

} // namespace Core

#endif // CORE_LASTACTIVITYCOMPARATOR_H

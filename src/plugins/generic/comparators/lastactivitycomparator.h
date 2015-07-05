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
	virtual int compare(qutim_sdk_0_3::Contact *a, qutim_sdk_0_3::Contact *b);
protected:
	virtual void doStartListen(qutim_sdk_0_3::Contact *contact);
};

} // namespace Core

#endif // CORE_LASTACTIVITYCOMPARATOR_H

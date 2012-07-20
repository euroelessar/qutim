#ifndef CORE_STATUSCOMPARATOR_H
#define CORE_STATUSCOMPARATOR_H
#include <qutim/contact.h>

namespace Core {

class StatusComparator : public qutim_sdk_0_3::ContactComparator
{
    Q_OBJECT
    Q_CLASSINFO("SettingsDescription", "Sort by contact's status")
public:
    explicit StatusComparator();
    virtual int compare(qutim_sdk_0_3::Contact *a, qutim_sdk_0_3::Contact *b);
protected:
    virtual void doStartListen(qutim_sdk_0_3::Contact *contact);
    virtual void doStopListen(qutim_sdk_0_3::Contact *contact);
};

} // namespace Core

#endif // CORE_STATUSCOMPARATOR_H

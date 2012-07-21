#include "lastactivitycomparator.h"

namespace Core {

LastActivityComparator::LastActivityComparator()
{
    //qDebug("%s", Q_FUNC_INFO);
}

int LastActivityComparator::compare(qutim_sdk_0_3::Contact *a, qutim_sdk_0_3::Contact *b)
{
    return StatusComparator::compare(a, b);
}

void LastActivityComparator::doStartListen(qutim_sdk_0_3::Contact *contact)
{
    StatusComparator::doStartListen(contact);
}

} // namespace Core

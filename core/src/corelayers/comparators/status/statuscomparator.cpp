#include "statuscomparator.h"

namespace Core {

StatusComparator::StatusComparator()
{

}

int StatusComparator::compare(qutim_sdk_0_3::Contact *a, qutim_sdk_0_3::Contact *b)
{
    int result = a->status().type() - b->status().type();
    if (result)
        return result;
    return a->title().compare(b->title(), Qt::CaseInsensitive);
}

void StatusComparator::doStartListen(qutim_sdk_0_3::Contact *contact)
{
}

void StatusComparator::doStopListen(qutim_sdk_0_3::Contact *contact)
{
}

} // namespace Core

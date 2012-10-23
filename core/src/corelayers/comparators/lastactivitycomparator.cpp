#include "lastactivitycomparator.h"

namespace Core {

LastActivityComparator::LastActivityComparator()
{
	Q_UNUSED(QT_TRANSLATE_NOOP("ContactList", "Sort by contact's last activity"));
}

int LastActivityComparator::compare(Ureen::Contact *a, Ureen::Contact *b)
{
	int result = b->lastActivity().toTime_t() - a->lastActivity().toTime_t();
	if (result)
		return result;
    return StatusComparator::compare(a, b);
}

void LastActivityComparator::doStartListen(Ureen::Contact *contact)
{
    StatusComparator::doStartListen(contact);
	connect(contact, SIGNAL(lastActivityChanged(QDateTime,QDateTime)), SLOT(onContactChanged()));
}

} // namespace Core

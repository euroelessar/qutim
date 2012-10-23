#include "statuscomparator.h"

namespace Core {

StatusComparator::StatusComparator()
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ContactList", "Sort by contact's status"));
}

int StatusComparator::compare(Ureen::Contact *a, Ureen::Contact *b)
{
	int result = a->status().type() - b->status().type();
	if (result)
		return result;
	return a->title().compare(b->title(), Qt::CaseInsensitive);
}

void StatusComparator::doStartListen(Ureen::Contact *contact)
{
	connect(contact, SIGNAL(nameChanged(QString,QString)), SLOT(onContactChanged()));
	connect(contact, SIGNAL(statusChanged(Ureen::Status,Ureen::Status)), SLOT(onContactChanged()));
}

void StatusComparator::doStopListen(Ureen::Contact *contact)
{
	contact->disconnect(this);
}

void StatusComparator::onContactChanged()
{
	emit contactChanged(static_cast<Ureen::Contact*>(sender()));
}

} // namespace Core

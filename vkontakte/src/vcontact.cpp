#include "vcontact.h"

VContact::VContact(const QString& id, Account* account): Contact(account)
{

}


QString VContact::id() const
{
	return QString();
}

bool VContact::isInList() const
{
	return true;
}

void VContact::sendMessage(const Message& message)
{

}

void VContact::setTags(const QSet< QString >& tags)
{

}

void VContact::setInList(bool inList)
{

}

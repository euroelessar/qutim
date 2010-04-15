#include "vcontact.h"

struct VContactPrivate
{
	bool online;
	QString id;
	bool inList;
	QSet<QString> tags;
	QString name;
};


VContact::VContact(const QString& id, Account* account): Contact(account), d_ptr(new VContactPrivate)
{
	d_func()->id = id;
}


QString VContact::id() const
{
	return d_func()->id;
}

bool VContact::isInList() const
{
	return d_func()->inList;
}

void VContact::sendMessage(const Message& message)
{

}

void VContact::setTags(const QSet< QString >& tags)
{
	d_func()->tags = tags;
}

void VContact::setInList(bool inList)
{
	d_func()->inList = inList;
}

Status VContact::status() const
{
	Status status (d_func()->online ? Status::Online : Status::Offline);
	status.initIcon("vkontakte");
	return status;
}

void VContact::setStatus(bool online)
{
	d_func()->online = online;
}

VContact::~VContact()
{

}

QSet< QString > VContact::tags() const
{
	return d_func()->tags;
}

QString VContact::name() const
{
	return d_func()->name;
}
void VContact::setName(const QString& name)
{
	d_func()->name = name;
}


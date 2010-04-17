#include "vcontact.h"
#include "vconnection.h"
#include "vaccount.h"
#include "vmessages.h"

struct VContactPrivate
{
	bool online;
	QString id;
	bool inList;
	QSet<QString> tags;
	QString name;
	VAccount *account;
};


VContact::VContact(const QString& id, VAccount* account): Contact(account), d_ptr(new VContactPrivate)
{
	Q_D(VContact);
	d->id = id;
	d->account = account;
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
	d_func()->account->connection()->messages()->sendMessage(message);
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
	Q_D(VContact);
	if (d->online != online) {
		d->online = online;
		emit statusChanged(status());
	}
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


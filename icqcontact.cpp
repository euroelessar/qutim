#include "icqcontact_p.h"
#include "icqaccount.h"
#include "roster.h"

IcqContact::IcqContact(const QString &uin, IcqAccount *account) : Contact(account), p(new IcqContactPrivate)
{
	p->account = account;
	p->uin = uin;
	p->status = Offline;
}

QSet<QString> IcqContact::tags() const
{
	QSet<QString> group;
	QString group_name = p->account->roster()->groupId2Name(p->group_id);
	if(!group_name.isNull())
		group.insert(group_name);
	return group;
}

QString IcqContact::id() const
{
	return p->uin;
}

QString IcqContact::name() const
{
	return p->name;
}

Status IcqContact::status() const
{
	return p->status;
}

// TODO: Impl this fucked things)

void IcqContact::sendMessage(const Message &message)
{
}

void IcqContact::setName(const QString &name)
{
}

void IcqContact::setTags(const QSet<QString> &tags)
{
}

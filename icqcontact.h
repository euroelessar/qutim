#ifndef ICQCONTACT_H
#define ICQCONTACT_H

#include <qutim/contact.h>

using namespace qutim_sdk_0_3;

class IcqAccount;
class Roster;

struct IcqContactPrivate;

class IcqContact : public Contact
{
public:
	IcqContact(const QString &uin, IcqAccount *account);
	virtual QSet<QString> tags() const;
	virtual QString id() const;
	virtual QString name() const;
	virtual Status status() const;
	virtual void sendMessage(const Message &message);
	virtual void setName(const QString &name);
	virtual void setTags(const QSet<QString> &tags);
private:
	friend class Roster;
	QScopedPointer<IcqContactPrivate> p;
};

#endif // ICQCONTACT_H

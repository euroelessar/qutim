#ifndef VCONTACT_H
#define VCONTACT_H
#include "vkontakte_global.h"
#include <qutim/contact.h>

class LIBVKONTAKTE_EXPORT VContact : public Contact
{
	Q_OBJECT
public:
	VContact(const QString &id,Account* account);
	virtual QString id() const;
	virtual bool isInList() const;
	virtual void sendMessage(const Message& message);
	virtual void setTags(const QSet< QString >& tags);
	virtual void setInList(bool inList);
};

#endif // VCONTACT_H

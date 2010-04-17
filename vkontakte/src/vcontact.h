#ifndef VCONTACT_H
#define VCONTACT_H
#include "vkontakte_global.h"
#include <qutim/contact.h>

class VAccount;
struct VContactPrivate;
class LIBVKONTAKTE_EXPORT VContact : public Contact
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VContact)
public:
	VContact(const QString& id, VAccount* account);
	virtual QString id() const;
	virtual bool isInList() const;
	virtual void sendMessage(const Message& message);
	virtual void setTags(const QSet< QString >& tags);
	virtual void setInList(bool inList);
	void setStatus(bool online);
	virtual Status status() const;
	virtual ~VContact();
	virtual QSet< QString > tags() const;
	virtual QString name() const;
	virtual void setName(const QString& name);
private:
	QScopedPointer<VContactPrivate> d_ptr;
};

#endif // VCONTACT_H

#ifndef VKONTAKTEACCOUNT_H
#define VKONTAKTEACCOUNT_H
#include <qutim/account.h>
#include "vkontakte_global.h"

struct VAccountPrivate;
class VContact;
class VConnection;
class LIBVKONTAKTE_EXPORT VAccount : public Account
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VAccount)
public:
	VAccount(const QString& email);
	virtual VContact* getContact(const QString& uid, bool create = false);
	virtual ChatUnit* getUnit(const QString& unitId, bool create = false) {return getUnit(unitId,create);};
	QString uid() const;
	QString email() const {return id();}; //alias for id
	void setUid(const QString &uid);
	virtual void setStatus(Status status);
	virtual ~VAccount();
	VConnection *connection();
	const VConnection *connection() const;
public slots:
	void updateSettings();
protected:
	QString password();
private:
	QScopedPointer<VAccountPrivate> d_ptr;
	friend class VConnection;
};

#endif // VKONTAKTEACCOUNT_H

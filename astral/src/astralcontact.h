#ifndef ASTRALCONTACT_H
#define ASTRALCONTACT_H

#include <qutim/contact.h>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/Contact>
#include <TelepathyQt4/Connection>

using namespace Tp;
using namespace qutim_sdk_0_3;

struct AstralContactPrivate;
class AstralAccount;

enum AstralStatusType
{
	AstralUnset         = 0,
	AstralOffline       = 1,
	AstralAvailable     = 2,
	AstralAway          = 3,
	AstralExtendedAway  = 4,
	AstralHidden        = 5,
	AstralBusy          = 6,
	AstralUnknown       = 7,
	AstralError         = 8
};

class AstralContact : public qutim_sdk_0_3::Contact
{
	Q_OBJECT
public:
	AstralContact(AstralAccount *acc, const ContactPtr &impl);
	virtual ~AstralContact();
	virtual QString id() const;
	virtual QStringList tags() const;
	virtual QString name() const;
	virtual Status status() const;
	virtual bool sendMessage(const qutim_sdk_0_3::Message &message);
	virtual void setName(const QString &name);
	virtual void setTags(const QStringList &tags);
	virtual bool isInList() const;
	virtual void setInList(bool inList);
	const ContactPtr &ptr();
private slots:
	void onStatusChanged();
	void onGroupsChanged();
private:
	QScopedPointer<AstralContactPrivate> p;
};

#endif // ASTRALCONTACT_H

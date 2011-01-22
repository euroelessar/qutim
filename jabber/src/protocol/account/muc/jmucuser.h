#ifndef JMUCUSER_H
#define JMUCUSER_H

#include "../roster/jcontactresource.h"
#include <jreen/mucroom.h>

namespace Jabber
{
using namespace jreen;

class JAccount;
class JMUCSession;
class JMUCUserPrivate;

class JMUCUser : public JContactResource
{
	Q_OBJECT
	Q_PROPERTY(QString realJid READ realJid)
	Q_DECLARE_PRIVATE(JMUCUser)
public:
	JMUCUser(JMUCSession *muc, const QString &name);
	~JMUCUser();
	QString title() const;
	QString name() const;
	QString avatar() const;
	QString avatarHash() const;
	void setAvatar(const QString &hex);
	void setName(const QString &name);
	//			InfoRequest *infoRequest() const;
	JMUCSession *muc() const;
	ChatUnit *upperUnit();
	MUCRoom::Affiliation affiliation();
	void setMUCAffiliation(MUCRoom::Affiliation affiliation);
	MUCRoom::Role role();
	void setMUCRole(MUCRoom::Role role);
	QString realJid() const;
	void setRealJid(const QString &jid);
	bool sendMessage(const qutim_sdk_0_3::Message &message);
protected:
	bool event(QEvent *ev);
	friend class JMUCSession;
};
}

#endif // JMUCUSER_H

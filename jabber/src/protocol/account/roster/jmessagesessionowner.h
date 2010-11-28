#ifndef JMESSAGESESSIONOWNER_H
#define JMESSAGESESSIONOWNER_H

#include <QObject>

namespace Jabber
{
class JMessageSession;
class JMessageSessionOwnerPrivate;

class JMessageSessionOwner
{
public:
	JMessageSessionOwner();
	~JMessageSessionOwner();
	JMessageSession *session();
private:
	friend class JMessageSession;
	void messageSessionCreated(JMessageSession *session);
	QScopedPointer<JMessageSessionOwnerPrivate> d;
};

}

Q_DECLARE_INTERFACE(Jabber::JMessageSessionOwner, "org.qutim.jabber.JMessageSessionOwner");

#endif // JMESSAGESESSIONOWNER_H

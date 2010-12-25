#include "jmessagesessionowner.h"
#include "jmessagesession.h"
#include "../muc/jmucuser.h"

//namespace Jabber
//{
//	class JMessageSessionOwnerPrivate
//	{
//	public:
//		QPointer<JMessageSession> session;
//	};

//	JMessageSessionOwner::JMessageSessionOwner() :
//		d(new JMessageSessionOwnerPrivate)
//	{
//	}

//	JMessageSessionOwner::~JMessageSessionOwner()
//	{
//		if (d->session)
//			d->session->deleteLater();
//	}

//	JMessageSession *JMessageSessionOwner::session()
//	{
//		return d->session.data();
//	}

//	void JMessageSessionOwner::messageSessionCreated(JMessageSession *session)
//	{
//		if (d->session)
//			d->session->deleteLater(); // ???
//		d->session = session;
//	}
//}

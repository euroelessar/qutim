#include "jmucuser.h"
#include "jmucsession.h"
#include "../jaccount.h"
#include "../roster/jcontactresource_p.h"
#include <QStringBuilder>

namespace Jabber
{
	class JMUCUserPrivate : public JContactResourcePrivate
	{
	public:
	};

	JMUCUser::JMUCUser(JMUCSession *muc, const QString &name) :
		JContactResource(muc, *new JMUCUserPrivate)
	{
		Q_D(JMUCUser);
		d->name = name;
		d->id = muc->id() % QLatin1Char('%') % name;
	}

	JMUCUser::~JMUCUser()
	{
	}

	QString JMUCUser::title() const
	{
		return d_func()->name;
	}
}

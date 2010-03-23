#ifndef JCONTACT_P_H
#define JCONTACT_P_H

#include "jcontact.h"

namespace Jabber
{
	struct JContactPrivate
	{
		~JContactPrivate()
		{
			qDeleteAll(resources);
		}
		JAccount *account;
		QHash<QString, JContactResource *> resources;
		QStringList currentResources;
		QSet<QString> tags;
		QString name;
		QString jid;
		bool inList;
		QString avatar;
		QStringRef hash;
	};
}
#endif // JCONTACT_P_H

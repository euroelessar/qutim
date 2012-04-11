/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#ifndef JABBER_JACCOUNTRESOURCE_H
#define JABBER_JACCOUNTRESOURCE_H

#include "jcontactresource.h"
#include "jcontact.h"
#include <qutim/account.h>

namespace Jabber {

class JAccount;
class JAccountResourcePrivate;

class JAccountResource : public JContact
{
	Q_OBJECT
public:
	JAccountResource(JAccount *account, const QString &id, const QString &resource);
	~JAccountResource();
	
	void setName(const QString &name);
	void setTags(const QStringList &tags);
	void setInList(bool inList);

private slots:
	void onNameChanged(const QString &name);

private:
	QString m_resource;
};

} // namespace Jabber

#endif // JABBER_JACCOUNTRESOURCE_H

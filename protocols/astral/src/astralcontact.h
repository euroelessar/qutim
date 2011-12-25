/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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


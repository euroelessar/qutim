/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef JPERSONACTIVITYCONVERTER_H
#define JPERSONACTIVITYCONVERTER_H

#include "../../../sdk/jabber.h"
#include <qutim/localizedstring.h>
#include <qutim/extensionicon.h>
#include <jreen/activity.h>

namespace Jabber {

using namespace qutim_sdk_0_3;

class JPersonActivityConverter : public QObject , public PersonEventConverter
{
	Q_OBJECT
	Q_INTERFACES(Jabber::PersonEventConverter)
public:
	JPersonActivityConverter();
	virtual ~JPersonActivityConverter();
	virtual QString name() const;
	virtual int entityType() const;
	virtual QSharedPointer<Jreen::Payload> convertTo(const QVariantHash &map) const;
	virtual QVariantHash convertFrom(const QSharedPointer<Jreen::Payload> &entity) const;
	static qutim_sdk_0_3::LocalizedString generalTitle(Jreen::Activity::General general);
	static qutim_sdk_0_3::LocalizedString specificTitle(Jreen::Activity::Specific specific);
};

class JPersonActivityRegistrator : public QObject
{
	Q_OBJECT
public:
	JPersonActivityRegistrator();
protected:
	bool eventFilter(QObject *obj, QEvent *event);
};

} // namespace Jabber

#endif // JPERSONACTIVITYCONVERTER_H


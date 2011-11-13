/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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


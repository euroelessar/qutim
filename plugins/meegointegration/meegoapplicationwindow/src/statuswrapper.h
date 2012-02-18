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

#ifndef STATUSWRAPPER_H
#define STATUSWRAPPER_H

#include <qutim/status.h>

namespace MeegoIntegration
{
class StatusWrapper : public QObject
{
    Q_OBJECT
	Q_ENUMS(Type ChangeReason)
public:
	enum Type
	{
		Connecting = qutim_sdk_0_3::Status::Connecting,
		Online = qutim_sdk_0_3::Status::Online,
		FreeChat = qutim_sdk_0_3::Status::FreeChat,
		Away = qutim_sdk_0_3::Status::Away,
		NA = qutim_sdk_0_3::Status::NA,
		DND = qutim_sdk_0_3::Status::DND,
		Invisible = qutim_sdk_0_3::Status::Invisible,
		Offline = qutim_sdk_0_3::Status::Offline
	};

	enum ChangeReason
	{
		ByUser = qutim_sdk_0_3::Status::ByUser,
		ByIdle = qutim_sdk_0_3::Status::ByIdle,
		ByAuthorizationFailed = qutim_sdk_0_3::Status::ByAuthorizationFailed,
		ByNetworkError = qutim_sdk_0_3::Status::ByNetworkError,
		ByFatalError = qutim_sdk_0_3::Status::ByFatalError
	};
	
    explicit StatusWrapper(QObject *parent = 0);

signals:

public slots:

};
}

#endif // STATUSWRAPPER_H


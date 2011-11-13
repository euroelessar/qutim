/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H
#include <qutim/plugin.h>

namespace qutim_sdk_0_3
{
namespace Authorization
{
class Reply;
}
}

namespace Core {

using namespace qutim_sdk_0_3;
using namespace Authorization;

class AuthService : public QObject
{
	Q_CLASSINFO("Service","AuthorizationService")
	Q_OBJECT
public:
protected:
	bool event(QEvent *event);
protected:
	void handleReply(Reply *reply);
private slots:
	void onAccepted();
	void onRejected();
};

} // namespace Core

#endif // AUTHSERVICE_H


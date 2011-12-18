/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef QUICKAUTHDIALOG_H
#define QUICKAUTHDIALOG_H

#include <qutim/authorizationdialog.h>
#include <qutim/notification.h>
#include <qutim/contact.h>

namespace qutim_sdk_0_3
{
namespace Authorization
{
class Reply;
}
}

namespace MeegoIntegration
{

using namespace qutim_sdk_0_3;
using namespace Authorization;

class QuickAuthDialog : public QObject {
	Q_CLASSINFO("Service","AuthorizationService")
	Q_OBJECT

public:
	QuickAuthDialog();
public:
	void accept(const QString &message, bool isIncoming, qutim_sdk_0_3::Contact* c);
	void cancel(Contact *c, bool isIncoming);
protected:
	bool event(QEvent *event);
protected:
	void handleReply(Reply *reply);

};
}

#endif /* QUICKPASSWORDDIALOG_H*/


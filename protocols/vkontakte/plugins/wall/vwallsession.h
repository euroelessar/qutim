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
#ifndef VWALLSESSION_H
#define VWALLSESSION_H

#include <QObject>
#include <qutim/conference.h>



using namespace qutim_sdk_0_3;
class VAccount;
class VWallSessionPrivate;
class VWallSession : public Conference
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VWallSession)
public:
	explicit VWallSession(const QString& id, VAccount *account);
	virtual QString id() const;
	virtual Buddy* me() const;
	virtual bool sendMessage(const qutim_sdk_0_3::Message& message);
    virtual ~VWallSession();
    virtual QString title() const;
protected:
	virtual void doJoin();
	virtual void doLeave();
private:
	QScopedPointer<VWallSessionPrivate> d_ptr;
};

#endif // VWALLSESSION_H


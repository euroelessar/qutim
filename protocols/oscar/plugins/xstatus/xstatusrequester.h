/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef XSTATUSREQUESTER_H
#define XSTATUSREQUESTER_H

#include <QTimer>
#include "../../src/icqcontact.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class XStatusRequester;

class XStatusRequesterList : public QObject
{
	Q_OBJECT
public:
	XStatusRequesterList();
	XStatusRequester *getRequester(IcqAccount *account);
private slots:
	void accountDestroyed(QObject *obj);
private:
	QHash<IcqAccount*, XStatusRequester*> m_requesters;
};

class XStatusRequester : public QObject
{
	Q_OBJECT
public:
	static void updateXStatus(IcqContact *contact);
private slots:
	void updateXStatus();
	void statusChanged(const qutim_sdk_0_3::Status &current, const qutim_sdk_0_3::Status &previous);
private:
	friend class XStatusRequesterList;
	XStatusRequester(IcqAccount *account);
	void updateXStatusImpl(IcqContact *contact);
	QList<QPointer<IcqContact> > m_contacts;
	QTimer m_timer;
	uint m_lastTime; // unix time when the last xstraz packet was sent
};

} } // namespace qutim_sdk_0_3::oscar

#endif // XSTATUSREQUESTER_H


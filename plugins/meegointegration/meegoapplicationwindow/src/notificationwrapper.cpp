/****************************************************************************
**
** qutIM instant messenger
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

#include "notificationwrapper.h"
#include <qdeclarative.h>
#include <qutim/debug.h>


namespace MeegoIntegration
{

Q_GLOBAL_STATIC(QList<NotificationWrapper*>, m_managers)
QuickNoficationManager* NotificationWrapper::m_currentManager;

NotificationWrapper::NotificationWrapper()
{
	m_managers()->append(this);
}

NotificationWrapper::~NotificationWrapper()
{
	m_managers()->removeOne(this);
}

void NotificationWrapper::init()
{
	qmlRegisterType<NotificationWrapper>("org.qutim", 0, 3, "Notifications");
	qmlRegisterUncreatableType<Notification>("org.qutim", 0, 3, "Notification",
											 QLatin1String("There is no public constructor"));
}

void NotificationWrapper::connect(QuickNoficationManager * manager)
{
	m_currentManager = manager;
	for (int i = 0; i < m_managers()->count();i++)
	{
		m_currentManager->setWindowActive(m_managers()->at(i)->windowActive());
	}
}

bool NotificationWrapper::windowActive()
{
	return m_windowActive;
}

void NotificationWrapper::setWindowActive(bool active)
{
	m_windowActive = active;
	if (m_currentManager)
	{
		m_currentManager->setWindowActive(active);
	}
	emit windowActiveChanged(active);
}

}

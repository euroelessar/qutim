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

#ifndef NOTIFICATIONWRAPPER_H
#define NOTIFICATIONWRAPPER_H

#include <QObject>
#include "quicknotificationmanager.h"

namespace MeegoIntegration
{

class NotificationWrapper : public QObject
{
	Q_OBJECT
	Q_PROPERTY(bool windowActive READ windowActive WRITE setWindowActive NOTIFY windowActiveChanged)
public:
	NotificationWrapper();
	~NotificationWrapper();
	bool windowActive();
	void setWindowActive(bool active);
	static void init();
	static void connect(QuickNoficationManager * manager);
	
public slots:
	static QString typeName(int type);
	static QString typeText(int type);
	static QString descriptionText(int type);
	static QStringList backendTypes();
	static QString backendDescription(const QString &backendType);

signals:
	void windowActiveChanged(bool active);

private:	
	bool m_windowActive;
	static QuickNoficationManager* m_currentManager;
};
}


#endif

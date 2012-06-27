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

#ifndef VibroBackend_H
#define VibroBackend_H

#include <qutim/notification.h>
#include <QtFeedback/QFeedbackFileEffect>

namespace Core
{

QTM_USE_NAMESPACE

class VibroBackend : public QObject, public qutim_sdk_0_3::NotificationBackend
{
	Q_OBJECT
	Q_CLASSINFO("Service", "Vibration")
	Q_PROPERTY(bool windowActive READ windowActive WRITE setWindowActive NOTIFY windowActiveChanged)
public:
	VibroBackend();
	virtual ~VibroBackend();
	virtual void handleNotification(qutim_sdk_0_3::Notification *notification);
	void setWindowActive(bool active);
	bool windowActive() const;
	
signals:
	void windowActiveChanged(bool windowActive);
	
private:
	QFeedbackFileEffect *m_strongEffect;
	QFeedbackFileEffect *m_effect;
	bool m_windowActive;
};

}

#endif // VibroBackend_H

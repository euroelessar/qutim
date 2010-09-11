//   Modern notifications, which use new Qt Statemachine and Animation framework
//   (c) by Sidorov "Sauron" Aleksey, sauron@citadelspb.com, 2010
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Library General Public
//   License version 2 or later as published by the Free Software Foundation.
//
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Library General Public License for more details.
//
//   You should have received a copy of the GNU Library General Public License
//   along with this library; see the file COPYING.LIB.  If not, write to
//   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
//   Boston, MA 02110-1301, USA.

#ifndef MANAGER_H
#define MANAGER_H
#include <QHash>
#include <QString>
#include <QSize>
#include <QEasingCurve>
#include "backend.h"

class QRect;
class QDir;
namespace Core
{
namespace KineticPopups
{

class Popup;
enum AnimationFlag
{
	NoAnimation = 0x0,
	Slide = 0x1,
	Opacity = 0x2
		  };
Q_DECLARE_FLAGS (AnimationFlags, AnimationFlag)
class Manager
{
	Q_DECLARE_FLAGS(NotificationTypes, Notifications::Type)
public:
			Manager();
	Popup *getById (const QString &id) const;
	Popup *getByNumber (const int &number) const;
	void loadSettings();
	int count();
	QRect insert (Popup *notification);
	void remove (const QString &id);
	void updateGeometry();
	static Manager *self();
	int animationDuration;
	QEasingCurve easingCurve;
	bool updatePosition;
	AnimationFlags animation;
	NotificationTypes showFlags;
	int timeout;
	bool appendMode;
	bool updateMode;
	int maxCount;
	int maxTextLength;
	uint margin;
	QString themeName;
private:
	QList<Popup *> active_notifications;
	static Manager *instance;
	int getNumber(const QString &id) const;
};
}
}
#endif // MANAGER_H

//   Modern notifications, which use new Qt Statemachine and Animation framework
//   (c) by Sidorov "Sauron" Aleksey, sauron@citadelspb.com, 2009
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

#ifndef NOTIFICATIONSMANAGER_H
#define NOTIFICATIONSMANAGER_H
#include <QHash>
#include <QString>
#include <QSize>
#include <QEasingCurve>
#include "kineticpopupbackend.h"
#include "kineticpopupthemehelper.h"
class QRect;
class KineticPopup;
class QDir;
class KineticPopupsManager
{
	Q_DECLARE_FLAGS(NotificationTypes, Notifications::Type)
public:
    KineticPopupsManager();
    KineticPopup *getById (const QString &id) const;
    KineticPopup *getByNumber (const int &number) const;
    QRect insert (KineticPopup *notification);
    void remove (const QString &id);
    void updateGeometry();
    static KineticPopupsManager *self();
    int animationDuration;
    QEasingCurve easingCurve;
    bool updatePosition;
    bool animation;
    Qt::MouseButton action1Trigger;
    Qt::MouseButton action2Trigger;
	NotificationTypes showFlags;
	uint timeout;
	bool appendMode;
	KineticPopupThemeHelper::PopupSettings popupSettings;
private:
    QList<KineticPopup *> active_notifications;
    static KineticPopupsManager *instance;
	void loadSettings();
    int getNumber(const QString &id) const;
};

#endif // NOTIFICATIONSMANAGER_H

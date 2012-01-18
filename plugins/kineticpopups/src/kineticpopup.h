/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef KINETICPOPUPS_KINETICPOPUP_H
#define KINETICPOPUPS_KINETICPOPUP_H
#include <QWidget>
#include <qutim/notification.h>

namespace KineticPopups {

class PopupWidget : public QWidget
{
	Q_OBJECT
public:
	PopupWidget(QWidget *parent = 0);
	virtual qutim_sdk_0_3::NotificationList notifications() const = 0;
	virtual void addNotification(qutim_sdk_0_3::Notification *notify) = 0;
	virtual void loadTheme(const QString &) {}
	virtual ~PopupWidget();
signals:
	void sizeChanged(QSize size);
	void finished();
};
typedef QList<PopupWidget*> PopupWidgetList;

} // namespace KineticPopups

#endif // KINETICPOPUPS_KINETICPOPUP_H


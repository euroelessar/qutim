/****************************************************************************
 *  kineticpopup.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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

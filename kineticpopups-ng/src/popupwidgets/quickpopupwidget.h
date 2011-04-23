/****************************************************************************
 *  quickpopupwidget.h
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

#ifndef KINETICPOPUPS_QUICKPOPUPWIDGET_H
#define KINETICPOPUPS_QUICKPOPUPWIDGET_H
#include "../kineticpopup.h"
#include <QPointer>

namespace KineticPopups {

class QuickPopupWidget : public PopupWidget
{
	Q_OBJECT
public:
    QuickPopupWidget(QWidget *parent = 0);
	virtual void addNotification(qutim_sdk_0_3::Notification* notify);
	virtual qutim_sdk_0_3::NotificationList notifications() const;
	virtual ~QuickPopupWidget();
	virtual QSize sizeHint() const;
private:
	qutim_sdk_0_3::NotificationList m_notifyList;
};

} // namespace KineticPopups

#endif // KINETICPOPUPS_QUICKPOPUPWIDGET_H

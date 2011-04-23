/****************************************************************************
 *  quickpopupwidget.cpp
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

#include "quickpopupwidget.h"
#include <QLabel>
#include <QVBoxLayout>

namespace KineticPopups {

using namespace qutim_sdk_0_3;

QuickPopupWidget::QuickPopupWidget(QWidget* parent): PopupWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowFlags(Qt::WindowStaysOnTopHint);
	setLayout(new QVBoxLayout(this));
}

NotificationList QuickPopupWidget::notifications() const
{
	return m_notifyList;
}

void QuickPopupWidget::addNotification(Notification* notify)
{
	m_notifyList.append(notify);
	QLabel *lbl = new QLabel(notify->request().text().left(100), this);
	lbl->setWordWrap(true);
	layout()->addWidget(lbl);
	setWindowTitle(notify->request().title());
	emit sizeChanged(sizeHint());
}

QuickPopupWidget::~QuickPopupWidget()
{

}

QSize QuickPopupWidget::sizeHint() const
{
	return QSize(250, 50);
}


} // namespace KineticPopups

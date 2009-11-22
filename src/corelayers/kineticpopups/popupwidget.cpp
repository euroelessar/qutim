/****************************************************************************
 *  popupwidget.cpp
 *
 *  Copyright (c) 2009 by Sidorov Aleksey <sauron@citadelspb.com>
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

#include "popupwidget.h"
#include <QVBoxLayout>
#include "manager.h"
#include <QMouseEvent>
#include <QDebug>
#include <QLabel>
#include <QPainter>
#include <QDesktopWidget>

namespace KineticPopups
{
	PopupWidget::PopupWidget (const ThemeHelper::PopupSettings &popupSettings,PopupWidgetFlags flags)
	{
		//init browser
		setTheme(popupSettings);
		if (flags & Preview) {
			setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
		}
		else {
			setWindowFlags(popup_settings.widgetFlags);
			//this->resize(NotificationsManager::self()->defaultSize);

			//init transparent
			setAttribute(Qt::WA_TranslucentBackground);
                        setAttribute(Qt::WA_NoSystemBackground, true);
			ensurePolished(); // workaround Oxygen filling the background
			setAttribute(Qt::WA_StyledBackground, false);
                        //hacks
		}
		setFrameShape ( QFrame::NoFrame );
		setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Expanding);
		setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
                setContextMenuPolicy(Qt::NoContextMenu);
		setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff);
	}


	QSize PopupWidget::setData ( const QString& title, const QString& body, const QString& imagePath )
	{
		QString data = popup_settings.content;
		data.replace ( "{title}", title );
		data.replace ( "{body}", body );
		data.replace ( "{imagepath}",Qt::escape ( imagePath ) );
		this->document()->setHtml(data);
		this->document()->setTextWidth(popup_settings.defaultSize.width());
		int width = popup_settings.defaultSize.width();
		int height = document()->size().height();

		return QSize(width,height);


	}


	void PopupWidget::setTheme (const ThemeHelper::PopupSettings &popupSettings )
	{
		popup_settings = popupSettings;
		this->setStyleSheet (popup_settings.styleSheet);
	}


	void PopupWidget::mouseReleaseEvent ( QMouseEvent* ev )
	{
		if (ev->button() == Manager::self()->action1Trigger) {
			emit action1Activated();
		}
		else if (ev->button() == Manager::self()->action2Trigger)
			emit action2Activated();
		else
			return;
		disconnect(SIGNAL(action1Activated()));
		disconnect(SIGNAL(action2Activated()));
	}
}

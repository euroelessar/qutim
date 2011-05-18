/****************************************************************************
 *  widgetplacer.h
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

#ifndef CORE_KINETICPOPUPS_WIDGETPLACER_H
#define CORE_KINETICPOPUPS_WIDGETPLACER_H

#include <QObject>

class QSize;
namespace qutim_sdk_0_3 {
class Notification;
}

namespace KineticPopups {

class PopupWidget;
typedef QList<PopupWidget*> PopupWidgetList;
class WidgetPlacerPrivate;
class WidgetPlacer : public QObject
{
    Q_OBJECT
	Q_DECLARE_PRIVATE(WidgetPlacer)
public:
	enum Corner
	{
		TopLeft,
		TopRight,
		BottomLeft,
		BottomRight
	};
	explicit WidgetPlacer(QObject *parent = 0);
	virtual ~WidgetPlacer();
	void addWidget(PopupWidget *widget);
	PopupWidgetList widgets() const;
	//PopupWidget *findPopupWidget(qutim_sdk_0_3::Notification *);
	void setCorner(Corner corner);
	Corner corner() const;
public slots:
	void updatePlace();
	void loadSettings();
private:
	QScopedPointer<WidgetPlacerPrivate> d_ptr;

	Q_PRIVATE_SLOT(d_func(), void _q_finished())
	Q_PRIVATE_SLOT(d_func(), void _q_destroyed(QObject*))
	Q_PRIVATE_SLOT(d_func(), void _q_size_changed(QSize))
	Q_PRIVATE_SLOT(d_func(), void _q_animation_destroyed(QObject*))
};

} // namespace KineticPopups

#endif // CORE_KINETICPOPUPS_WIDGETPLACER_H

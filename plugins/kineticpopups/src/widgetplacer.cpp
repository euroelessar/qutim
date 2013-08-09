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

#include "widgetplacer.h"
#include "kineticpopup.h"
#include <QPropertyAnimation>
#include <QApplication>
#include <QDesktopWidget>

#include <qutim/utils.h>
#include <qutim/config.h>
#include <qutim/debug.h>

namespace KineticPopups {

using namespace qutim_sdk_0_3;

class WidgetPlacer;
class WidgetPlacerPrivate
{
	Q_DECLARE_PUBLIC(WidgetPlacer)
public:
	WidgetPlacerPrivate(WidgetPlacer *q) : q_ptr(q) {}
	WidgetPlacer *q_ptr;
	PopupWidgetList popups;
	WidgetPlacer::Corner corner;
	int duration;
	int margin;
	QHash<PopupWidget*, QPropertyAnimation*> runningAnimations;

	QRect actualGeometry(PopupWidget *popup) const
	{
		if (QPropertyAnimation *a = runningAnimations.value(popup))
			return a->endValue().toRect();
		else
			return popup->geometry();
	}

	QRect calculateGeometry(int after, PopupWidget *widget) const
	{
		QSize size = widget->sizeHint();
		QRect desktopRect = QApplication::desktop()->availableGeometry(QCursor::pos());
		//place first widget
		if (after == -1 || after >= popups.count()) {
			switch (corner) {
			//TODO add additional corners
			case WidgetPlacer::BottomRight:
			default:
				QPoint topLeft(desktopRect.bottomRight().x() - margin - size.width(),
							   desktopRect.bottomRight().y() - margin - size.height());
				return QRect(topLeft, size);
			}
		}
		PopupWidget *beforeWidget = popups.at(after);
		switch (corner) {
		//TODO add additional corners
		case WidgetPlacer::BottomRight:
		default:
			QPoint topLeft(desktopRect.bottomRight().x() - margin - size.width(),
						   actualGeometry(beforeWidget).topRight().y() - margin - size.height());
			return QRect(topLeft, size);
		}
	}
	void doInsert(int after, PopupWidget *widget)
	{
		QRect geometry = calculateGeometry(after, widget);
		doMove(widget, geometry);
	}
	void doLayout(int first = 0)
	{
		if (!popups.count())
			return;
		//simple check
		if (first < 0 || first >= (popups.count())) {
			first = 0;
			qDebug() << "WidgetPlacer::invalid first popup";
		}

		for (int i = first; i < popups.count(); i++) {
			PopupWidget *widget = popups.at(i);
			doInsert(i - 1, widget);
		}
	}
	void doMove(PopupWidget *popup,const QRect &endGeometry)
	{
		Q_Q(WidgetPlacer);
		if (QPropertyAnimation *a = runningAnimations.value(popup))
			a->stop();

		QRect startGeometry;
		if (popup->isVisible())
			startGeometry = popup->geometry();
		else {
			startGeometry = endGeometry;
			startGeometry.translate(0, margin/2);
		}

		QPropertyAnimation *animation = new QPropertyAnimation(popup, "geometry", popup);
		animation->setDuration(duration);
		animation->setStartValue(startGeometry);
		animation->setEndValue(endGeometry);
		animation->start(QAbstractAnimation::DeleteWhenStopped);
		runningAnimations.insert(popup, animation);

		q->connect(animation, SIGNAL(destroyed(QObject*)),
				   SLOT(_q_animation_destroyed(QObject*)));
	}
	void doShow(PopupWidget *popup)
	{
		popup->show();
		QPropertyAnimation *animation = new QPropertyAnimation(popup, "windowOpacity", popup);
		animation->setDuration(duration);
		animation->setStartValue(0);
		animation->setEndValue(1);
		animation->start(QAbstractAnimation::DeleteWhenStopped);
	}
	void doHide(PopupWidget *popup, bool deleteOnClose = true)
	{
		QPropertyAnimation *animation = new QPropertyAnimation(popup, "windowOpacity", popup);
		animation->setDuration(duration);
		animation->setStartValue(1);
		animation->setEndValue(0);
		if (deleteOnClose)
			animation->connect(animation, SIGNAL(destroyed()), popup, SLOT(deleteLater()));
		animation->start(QAbstractAnimation::DeleteWhenStopped);
	}
	void _q_finished()
	{
		Q_Q(WidgetPlacer);
		PopupWidget *popup = sender_cast<PopupWidget*>(q->sender());
		doHide(popup);
	}
	void _q_destroyed(QObject *obj)
	{
		popups.removeAll(static_cast<PopupWidget*>(obj));
		doLayout();
	}
	void _q_size_changed(QSize)
	{
		//PopupWidget *popup = sender_cast<PopupWidget*>(q_func()->sender());
		doLayout();
		//doLayout(popups.indexOf(popup));
	}
	void _q_animation_destroyed(QObject *obj)
	{
		PopupWidget *key = runningAnimations.key(static_cast<QPropertyAnimation*>(obj));
		runningAnimations.remove(key);
	}
};


WidgetPlacer::WidgetPlacer(QObject* parent) :
	QObject(parent),
	d_ptr(new WidgetPlacerPrivate(this))
{
	loadSettings();
}

WidgetPlacer::~WidgetPlacer()
{

}

void WidgetPlacer::addWidget(PopupWidget* widget)
{
	Q_D(WidgetPlacer);
	d->popups.append(widget);
	d->doLayout();
	d->doShow(widget);

	connect(widget, SIGNAL(finished()), SLOT(_q_finished()));
	connect(widget, SIGNAL(destroyed(QObject*)), SLOT(_q_destroyed(QObject*)));
	connect(widget, SIGNAL(sizeChanged(QSize)), SLOT(_q_size_changed(QSize)));
}

WidgetPlacer::Corner WidgetPlacer::corner() const
{
	return d_func()->corner;
}

void WidgetPlacer::setCorner(WidgetPlacer::Corner corner)
{
	Q_D(WidgetPlacer);
	d->corner = corner;
	d->doLayout();
}

PopupWidgetList WidgetPlacer::widgets() const
{
	return d_func()->popups;
}

//PopupWidget *WidgetPlacer::findPopupWidget(qutim_sdk_0_3::Notification *notify)
//{
//	foreach (PopupWidget *widget, d_func()->popups) {
//		if (widget->notify() == notify)
//			return widget;
//	}
//	return 0;
//}

void WidgetPlacer::updatePlace()
{
	d_func()->doLayout();
}

void WidgetPlacer::loadSettings()
{
	Q_D(WidgetPlacer);
	Config cfg("behaviour");
	cfg.beginGroup("popup");
	d->corner = cfg.value("corner", BottomRight);
	d->duration = cfg.value("duration", 600);
	d->margin = cfg.value("margin", 20);
	cfg.endGroup();
}

} // namespace KineticPopups

#include "moc_widgetplacer.cpp"


/*****************************************************************************
	QtCustomWidget

	Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "qtcustomwidget_p.h"
#include <QPaintEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QPointer>
#include <QPainter>
#include <QDir>
#include <QSettings>
#include <QPixmap>
#include <QApplication>
#include <QDesktopWidget>

QT_BEGIN_NAMESPACE

struct QtCustomButtonPrivate
{
	QtCustomButton::State state;
	QtCustomButtonGeometry geometry;
	QPixmap hover;
	QPixmap pressed;
	QPixmap normal;
	bool is_pressed;
	bool send_on_press;
	QtCustomButton::Action action;
};


QtCustomButton::QtCustomButton(const QtCustomButtonGeometry &geometry, Action action, QWidget *parent)
	: QWidget(parent), p(new QtCustomButtonPrivate)
{
	setMouseTracking(true);
	p->geometry = geometry;
	p->state = Normal;
	p->is_pressed = false;
	p->send_on_press = true;
	p->action = action;
}

const QtCustomButtonGeometry &QtCustomButton::getGeometry()
{
	return p->geometry;
}

void QtCustomButton::setPixmaps(const QPixmap &normal, const QPixmap &hover)
{
	p->normal = p->pressed = normal;
	p->hover = hover;
}

QtCustomButton::~QtCustomButton()
{
	delete p;
}

void QtCustomButton::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	switch(p->state)
	{
	case Hover:
		painter.drawPixmap(event->rect(), p->hover, event->rect());
		break;
	case Pressed:
		painter.drawPixmap(event->rect(), p->pressed, event->rect());
		break;
	case Normal:
		painter.drawPixmap(event->rect(), p->normal, event->rect());
		break;
	}
}

void QtCustomButton::leaveEvent(QEvent *event)
{
	Q_UNUSED(event);
	setState(Normal);
}

void QtCustomButton::mouseMoveEvent(QMouseEvent *event)
{
	if(rect().contains(event->pos()))
		setState(p->is_pressed ? Pressed : Hover);
	else
		setState(Normal);
}

void QtCustomButton::mousePressEvent(QMouseEvent *event)
{
	if(event->button() != Qt::LeftButton)
		return;
	setState(Pressed);
	if(p->send_on_press)
	{
		switch(p->action)
		{
		case Minimize:
			parentWidget()->showMinimized();
			break;
		case Close:
			parentWidget()->close();
			break;
		}
	}
	else
		p->is_pressed = true;
}

void QtCustomButton::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button() != Qt::LeftButton)
		return;
	if(p->is_pressed)
	{
		if(p->state == Pressed)
		{
			switch(p->action)
			{
			case Minimize:
				parentWidget()->showMinimized();
				break;
			case Close:
				parentWidget()->close();
				break;
			}
		}
	}
	p->is_pressed = false;
	if(rect().contains(event->pos()))
		setState(Hover);
	else
		setState(Normal);
}

void QtCustomButton::setState(State state)
{
	if(p->state == state)
		return;
	p->state = state;
	update();
}

class QtCustomWidgetPrivate
{
public:
	QtCustomWidgetPrivate();
	QPointer<QWidget> widget;
	struct Margin
	{
		Margin() : l(0), r(0), t(0), b(0) {}
		int l;
		int r;
		int t;
		int b;
	} margin;
	enum PosType
	{
		PosNone   = 0x00,
		PosLeft   = 0x01,
		PosTop    = 0x02,
		PosRight  = 0x04,
		PosBottom = 0x08,
		PosCenter = 0x10,
		PosTopLeft     = PosTop    | PosLeft,
		PosTopRight    = PosTop    | PosRight,
		PosBottomLeft  = PosBottom | PosLeft,
		PosBottomRight = PosBottom | PosRight
	};
	enum MoveAction
	{
		ActionNone,
		ActionResize,
		ActionMove
	};
	inline PosType getCursor(const QSize &size, const QPoint &pos);
	void paintEvent(QPaintEvent *event);
	void leaveEvent(QEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseDoubleClickEvent(QMouseEvent *event);
	void resizeEvent(QResizeEvent *event);
	void startMoveAction(const QRect &geom, const QPoint &pos, PosType type);
	void moveEvent(const QPoint &pos);
	void stopMoveAction();
	inline QRect getGeometry(const QSize &size, const QtCustomButtonGeometry &geom);
	inline int getPosition(int margin, int right, QtCustomButtonGeometry::Type type);
	int left_resize_min;
	int left_resize_max;
	int top_resize_min;
	int top_resize_max;
	int right_resize_min;
	int right_resize_max;
	int bottom_resize_min;
	int bottom_resize_max;
	int margin_right;
	int margin_top;
	int margin_bottom;
	int margin_left;
	struct Borders
	{
		QPixmap left_top;
		QPixmap top;
		QPixmap right_top;
		QPixmap left;
		QPixmap right;
		QPixmap left_bottom;
		QPixmap bottom;
		QPixmap right_bottom;
	};
	Borders borders;
	QColor background_color;
	MoveAction move_action;
	QRect geometry;
	QPoint point;
	PosType position_type;
	QtCustomButton *button_quit;
	QtCustomButton *button_min;
	Qt::WindowFlags flags;
};

QtCustomWidgetPrivate::QtCustomWidgetPrivate()
{
	left_resize_min = 0;
	left_resize_max = 3;
	top_resize_min = 0;
	top_resize_max = 3;
	right_resize_min = 0;
	right_resize_max = 3;
	bottom_resize_min = 0;
	bottom_resize_max = 3;
	button_min = 0;
	button_quit = 0;
	margin_right = 0;
	margin_top = 0;
	margin_bottom = 0;
	margin_left = 0;
	move_action = ActionNone;
}

QtCustomWidgetPrivate::PosType QtCustomWidgetPrivate::getCursor(const QSize &size, const QPoint &pos)
{
	int cur = 0;
	cur |= pos.x() <= left_resize_max && pos.x() >= left_resize_min ? PosLeft : 0;
	cur |= pos.y() <= top_resize_max  && pos.y() >= top_resize_min  ? PosTop  : 0;
	cur |= size.width()-pos.x() <= right_resize_max && size.width()-pos.x() >= right_resize_min ? PosRight : 0;
	cur |= size.height()-pos.y() <= bottom_resize_max  && size.height()-pos.y() >= bottom_resize_min  ? PosBottom  : 0;
	return static_cast<PosType>(cur);
}

void QtCustomWidgetPrivate::paintEvent(QPaintEvent *event)
{
	QPainter painter(widget);
	int top = borders.left_top.height();
	int bottom = borders.right_bottom.height();
	int left_top_width = borders.left_top.width();
	int right_top_width = borders.right_top.width();
	int left_bottom_width = borders.left_bottom.width();
	int right_bottom_width = borders.right_bottom.width();
	int left_width = borders.left.width();
	int right_width = borders.right.width();
	
	painter.fillRect(left_width , top, widget->width() - left_width - right_width, widget->height() - top - bottom, background_color);
	painter.drawPixmap(0, 0, left_top_width,borders.left_top.height(), borders.left_top);
	painter.drawPixmap(left_top_width, 0, widget->width() - left_top_width - right_top_width, top, borders.top);
	painter.drawPixmap(widget->width() - right_top_width, 0, borders.right_top);
	painter.drawPixmap(0, top, left_width, widget->height() - top - bottom, borders.left);
	painter.drawPixmap(0, widget->height() - bottom, borders.left_bottom);
	painter.drawPixmap(left_bottom_width, widget->height() - bottom, widget->width() - left_bottom_width - right_bottom_width, bottom, borders.bottom);
	painter.drawPixmap(widget->width() - right_bottom_width, widget->height() - bottom, borders.right_bottom);
	painter.drawPixmap(widget->width() - right_width, top, right_width, widget->height() - top - bottom, borders.right);
}

void QtCustomWidgetPrivate::leaveEvent(QEvent *event)
{
	widget->unsetCursor();
}

void QtCustomWidgetPrivate::mouseMoveEvent(QMouseEvent *event)
{
	PosType cursor;
	if(!widget->geometry().contains(event->globalPos()))
		cursor = PosNone;
	else
		cursor = getCursor(widget->size(), event->pos());
	switch(cursor)
	{
	case PosTopLeft:
	case PosBottomRight:
		widget->setCursor(Qt::SizeFDiagCursor);
		break;
	case PosTopRight:
	case PosBottomLeft:
		widget->setCursor(Qt::SizeBDiagCursor);
		break;
	case PosLeft:
	case PosRight:
		widget->setCursor(Qt::SizeHorCursor);
		break;
	case PosTop:
	case PosBottom:
		widget->setCursor(Qt::SizeVerCursor);
		break;
	default:
		widget->unsetCursor();
	}
	moveEvent(event->globalPos());
}

void QtCustomWidgetPrivate::mousePressEvent(QMouseEvent *event)
{
	if(event->button()!=Qt::LeftButton)
		return;
	startMoveAction(widget->geometry(), event->globalPos(), getCursor(widget->size(), event->pos()));
}

void QtCustomWidgetPrivate::mouseReleaseEvent(QMouseEvent *event)
{
	if(event->button()!=Qt::LeftButton)
		return;
	stopMoveAction();
}

void QtCustomWidgetPrivate::mouseDoubleClickEvent(QMouseEvent *event)
{
	// TODO: May be maximize at double click
	Q_UNUSED(event);
}

void QtCustomWidgetPrivate::resizeEvent(QResizeEvent *event)
{
	Q_UNUSED(event);
	button_quit->setGeometry(getGeometry(event->size(), button_quit->getGeometry()));
	button_min->setGeometry(getGeometry(event->size(), button_min->getGeometry()));
}

void QtCustomWidgetPrivate::startMoveAction(const QRect &geom, const QPoint &pos, QtCustomWidgetPrivate::PosType type)
{
	if(move_action != ActionNone)
		return;
	geometry = geom;
	point = pos;
	position_type = type;
	if(type == PosNone)
		move_action = ActionMove;
	else
		move_action = ActionResize;
}

void QtCustomWidgetPrivate::moveEvent(const QPoint &pos)
{
	if(move_action == ActionMove)
	{
		widget->move(geometry.topLeft()-point+pos);
	}
	else if(move_action == ActionResize)
	{
		QRect geom = geometry;
		if(position_type & PosLeft)
			geom.setLeft(geom.left()-point.x()+pos.x());
		else if(position_type & PosRight)
			geom.setRight(geom.right()-point.x()+pos.x());
		if(position_type & PosTop)
			geom.setTop(geom.top()-point.y()+pos.y());
		else if(position_type & PosBottom)
			geom.setBottom(geom.bottom()-point.y()+pos.y());
		widget->setGeometry(geom);
	}
}

void QtCustomWidgetPrivate::stopMoveAction()
{
	move_action = ActionNone;
}


int QtCustomWidgetPrivate::getPosition(int margin, int right, QtCustomButtonGeometry::Type type)
{
	switch(type)
	{
	case QtCustomButtonGeometry::LeftToCenter:
		return margin;
	case QtCustomButtonGeometry::Center:
		return right/2+margin;
		break;
	case QtCustomButtonGeometry::RightToCenter:
		return right-margin;
		break;
	};
	return margin;
}

QRect QtCustomWidgetPrivate::getGeometry(const QSize &size, const QtCustomButtonGeometry &geom)
{
	QPoint lt(getPosition(geom.left_m, size.width(), geom.left_t), getPosition(geom.top_m, size.height(), geom.top_t));
	QPoint rb(getPosition(geom.right_m, size.width(), geom.right_t) - 1, getPosition(geom.bottom_m, size.height(), geom.bottom_t) - 1);
	return QRect(lt, rb);
}

QtCustomWidget::QtCustomWidget(QWidget *parent) : QObject(parent), p(new QtCustomWidgetPrivate)
{
	Q_ASSERT_X(parent, "QtCustomWidget(QWidget *parent)", "Parent must be valid");
	p->widget = QPointer<QWidget>(parent);
	p->background_color = parent->palette().color(QPalette::Window);
	p->widget->setMouseTracking(true);
	p->flags = parent->windowFlags();
}

QtCustomWidget::~QtCustomWidget()
{
	if(p->widget)
		stop();
	delete p;
}

void QtCustomWidget::start(const QString &config_path)
{
	stop();
	QDir dir = config_path;
	if(!dir.cd("cl_border"))
		return;
	p->flags = p->widget->windowFlags();
#if (QT_VERSION >= QT_VERSION_CHECK(4,5,0))
	p->widget->setAttribute(Qt::WA_TranslucentBackground, true);
	p->widget->setMask(QApplication::desktop()->screenGeometry());
#endif
	p->widget->getContentsMargins(&p->margin_left, &p->margin_top, &p->margin_right, &p->margin_bottom);
	p->borders.left = QPixmap(dir.filePath("left"));
	p->borders.top = QPixmap(dir.filePath("up"));
	p->borders.right = QPixmap(dir.filePath("right"));
	p->borders.bottom = QPixmap(dir.filePath("down"));
	p->borders.left_top = QPixmap(dir.filePath("up_left"));
	p->borders.left_bottom = QPixmap(dir.filePath("down_left"));
	p->borders.right_top = QPixmap(dir.filePath("up_right"));
	p->borders.right_bottom = QPixmap(dir.filePath("down_right"));
	p->widget->setContentsMargins(p->borders.left.width(), p->borders.left_top.height(), p->borders.right.width(), p->borders.right_bottom.height());

	p->widget->setWindowFlags(Qt::FramelessWindowHint);
	QSettings settings(dir.filePath("config.ini"), QSettings::defaultFormat());
	settings.beginGroup("buttons");
//	bool up_layout = settings.value("up", true).toBool();
	int margin_down = settings.value("mdown", -1).toInt();
	int margin_up = settings.value("mup", -1).toInt();
	int margin_left = settings.value("mleft", -1).toInt();
	int margin_right = settings.value("mright", -1).toInt();

	int margin_width;
	int margin_height;

	QtCustomButtonGeometry geometry;
	if(margin_left < 0)
	{
		margin_width = margin_right;
		geometry.left_t = QtCustomButtonGeometry::RightToCenter;
		geometry.right_t = QtCustomButtonGeometry::RightToCenter;
	}
	else
	{
		margin_width = margin_left;
		geometry.left_t = QtCustomButtonGeometry::LeftToCenter;
		geometry.right_t = QtCustomButtonGeometry::LeftToCenter;
	}
	if(margin_down < 0)
	{
		margin_height = margin_up;
		geometry.bottom_t = QtCustomButtonGeometry::LeftToCenter;
		geometry.top_t = QtCustomButtonGeometry::LeftToCenter;
	}
	else
	{
		margin_height = margin_down;
		geometry.bottom_t = QtCustomButtonGeometry::RightToCenter;
		geometry.top_t = QtCustomButtonGeometry::RightToCenter;
	}

	QPixmap quit_normal(dir.filePath("close"));
	QPixmap quit_hover(dir.filePath("close_hover"));

	geometry.left_m = margin_width + quit_normal.width();
	geometry.right_m = margin_width;
	geometry.bottom_m = margin_height + quit_normal.height();
	geometry.top_m = margin_height;
	if(margin_left >= 0)
		qSwap(geometry.left_m, geometry.right_m);
	if(margin_down >= 0)
		qSwap(geometry.bottom_t, geometry.top_t);

	p->button_quit = new QtCustomButton(geometry, QtCustomButton::Close, p->widget);
	p->button_quit->setPixmaps(quit_normal, quit_hover);
	p->button_quit->setGeometry(p->getGeometry(p->widget->size(), p->button_quit->getGeometry()));

	QPixmap min_normal(dir.filePath("minimise"));
	QPixmap min_hover(dir.filePath("minimise_hover"));
	if(margin_left >= 0)
		qSwap(geometry.left_m, geometry.right_m);
	if(margin_down >= 0)
		qSwap(geometry.bottom_t, geometry.top_t);

	geometry.left_m += min_normal.width() + 1;
	geometry.right_m += quit_normal.width() + 1;
	if(margin_left >= 0)
		qSwap(geometry.left_m, geometry.right_m);
	if(margin_down >= 0)
		qSwap(geometry.bottom_t, geometry.top_t);

	p->button_min = new QtCustomButton(geometry, QtCustomButton::Minimize, p->widget);
	p->button_min->setPixmaps(min_normal, min_hover);
	p->button_min->setGeometry(p->getGeometry(p->widget->size(), p->button_min->getGeometry()));

	QList<QWidget *> children = p->widget->findChildren<QWidget *>();
	foreach(QWidget *widget, children)
		widget->installEventFilter(this);

	settings.endGroup();
	settings.beginGroup("window");
	p->background_color.setNamedColor(settings.value("backcolor", p->background_color).toString());
	p->widget->installEventFilter(this);
}

void QtCustomWidget::stop()
{
	if(!p->button_min)
		return;
#if (QT_VERSION >= QT_VERSION_CHECK(4,5,0))
	p->widget->setAttribute(Qt::WA_TranslucentBackground, false);
	p->widget->setAttribute(Qt::WA_NoSystemBackground, false);
	p->widget->clearMask();
#endif
	p->widget->removeEventFilter(this);
	p->borders = QtCustomWidgetPrivate::Borders();
	p->widget->setContentsMargins(p->margin_left, p->margin_top, p->margin_right, p->margin_bottom);
	delete p->button_min;
	delete p->button_quit;
	p->button_min = 0;
	p->button_quit = 0;
	QList<QWidget *> children = p->widget->findChildren<QWidget *>();
	foreach(QWidget *widget, children)
		widget->removeEventFilter(this);
	p->widget->setWindowFlags(p->flags);
}

bool QtCustomWidget::eventFilter(QObject *obj, QEvent *event)
{
	if(obj != p->widget)
	{
		if(event->type() == QEvent::Enter)
			p->widget->unsetCursor();
		return QObject::eventFilter(obj, event);
	}
	switch(event->type())
	{
	case QEvent::Paint:
		p->paintEvent(static_cast<QPaintEvent *>(event));
		return true;
	case QEvent::Leave:
		p->leaveEvent(event);
		return true;
	case QEvent::MouseMove:
		p->mouseMoveEvent(static_cast<QMouseEvent *>(event));
		return true;
	case QEvent::MouseButtonPress:
		p->mousePressEvent(static_cast<QMouseEvent *>(event));
		return true;
	case QEvent::MouseButtonRelease:
		p->mouseReleaseEvent(static_cast<QMouseEvent *>(event));
		return true;
//	case QEvent::MouseButtonDblClick:
//		p->mouseDoubleClickEvent(static_cast<QMouseEvent *>(event));
//		return true;
	case QEvent::Resize:
		p->resizeEvent(static_cast<QResizeEvent *>(event));
		break;
	default:
		break;
	}
	return QObject::eventFilter(obj, event);
}

QT_END_NAMESPACE

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "contactwidget.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QDebug>
#include <QToolTip>
#include <QApplication>
#include <QDesktopWidget>
#include <QAbstractItemView>
#include <qutim/icon.h>
#include <qutim/chatsession.h>
#include <qutim/tooltip.h>

using namespace qutim_sdk_0_3;

class ItemViewHook : public QAbstractItemView
{
public:
	inline QStyleOptionViewItem viewOptionsHook() const
	{ return viewOptions(); }
};

ContactWidget::ContactWidget(const QPersistentModelIndex &index, QAbstractItemView *view,
							 Contact *contact)
								 : m_index(index), m_view(view), m_contact(contact),
								 m_mouse_pressed(false), m_position(0,0)
{
	connect(contact, SIGNAL(destroyed()), this, SLOT(deleteLater()));
	connect(contact, SIGNAL(avatarChanged(QString)), this, SLOT(update()));
	connect(contact, SIGNAL(titleChanged(QString,QString)), this, SLOT(update()));
	connect(contact, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(update()));
#if defined(Q_OS_MAC)
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
#else
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::ToolTip);
#endif
	QPalette pal = palette();
	pal.setColor(QPalette::Window, pal.color(QPalette::Base));
	setPalette(pal);
//#if (QT_VERSION >= QT_VERSION_CHECK(4,5,0))
//	setAttribute(Qt::WA_TranslucentBackground, true);
//	setMask(QApplication::desktop()->screenGeometry());
//#endif
	setAttribute(Qt::WA_AlwaysShowToolTips, true);
	setAttribute(Qt::WA_Hover, true);
	ensureSize();
}

ContactWidget::~ContactWidget()
{
}

void ContactWidget::contextMenuEvent(QContextMenuEvent * event)
{
	m_contact->showMenu(event->globalPos());
}

void ContactWidget::mousePressEvent(QMouseEvent *event)
{
    m_mouse_pressed = event->button() == Qt::LeftButton;
    m_position = event->pos();
	update();
}

void ContactWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        m_mouse_pressed = false;
		QWidget *widget = m_view->window();
		if (widget->isVisible() && widget->geometry().intersects(geometry()))
			emit wantDie(m_contact);
		else
			update();
	}
}

void ContactWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_mouse_pressed)
        move(event->globalPos() - m_position);
}

void ContactWidget::mouseDoubleClickEvent(QMouseEvent *)
{
	ChatLayer::get(m_contact, true)->activate();
}

void ContactWidget::wheelEvent(QWheelEvent *event)
{
    setWindowOpacity(windowOpacity()+event->delta()/2400.0);
}

void ContactWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	m_view->itemDelegate()->paint(&painter, viewOptionV4(), m_index);
}

void ContactWidget::ensureSize()
{
	resize(m_view->itemDelegate()->sizeHint(viewOptionV4(), m_index));
}

QStyleOptionViewItemV4 ContactWidget::viewOptionV4()
{
	QStyleOptionViewItemV4 option = static_cast<ItemViewHook*>(m_view)->viewOptionsHook();
	option.locale = this->locale();
	option.locale.setNumberOptions(QLocale::OmitGroupSeparator);
	option.widget = this;
	if (m_mouse_pressed)
		option.state |= QStyle::State_Selected;
	if (geometry().contains(QCursor::pos()))
		option.state |= QStyle::State_MouseOver;
	option.rect = QRect(0, 0, 200, height());
	return option;
}

bool ContactWidget::event(QEvent *event)
{
	if (event->type() == QEvent::ToolTip) {
		ToolTip::instance()->showText(static_cast<QHelpEvent*>(event)->globalPos(), m_contact, this);
		return true;
	}
	return QWidget::event(event);
}


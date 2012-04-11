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

#include "quickchatviewwidget.h"
#include "quickchatviewcontroller.h"
#include <qutim/config.h>
#include <QDeclarativeItem>
#ifndef QT_NO_OPENGL
#	include <QtOpenGL/QtOpenGL>
#endif

namespace Core {
namespace AdiumChat {

using namespace qutim_sdk_0_3;

QuickChatViewWidget::QuickChatViewWidget(QWidget *parent) :
	QGraphicsView(parent)
{
#ifndef QT_NO_OPENGL
	if (Config("appearance/qmlChat").value("openGL", false))
		setViewport(new QGLWidget(QGLFormat(QGLFormat::defaultFormat())));
#endif
	setOptimizationFlags(QGraphicsView::DontSavePainterState);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	// These seem to give the best performance
	setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

	viewport()->setFocusPolicy(Qt::NoFocus);
	setFocusPolicy(Qt::StrongFocus);
}

void QuickChatViewWidget::setViewController(QObject* object)
{
	if (m_controller.data() == object)
		return;
	if (m_controller)
		m_controller.data()->disconnect(this);
	QuickChatController* controller = qobject_cast<QuickChatController*>(object);
	m_controller = controller;
	if (controller) {
		controller->setItemIndexMethod(QGraphicsScene::NoIndex);
		controller->setStickyFocus(true);  //### needed for correct focus handling
		setScene(controller);
		updateView();
		connect(controller, SIGNAL(rootItemChanged(QDeclarativeItem*)),
				this, SLOT(onRootChanged(QDeclarativeItem*)));
	} else {
		setScene(new QGraphicsScene(this));
	}
}

void QuickChatViewWidget::resizeEvent(QResizeEvent *event)
{
	updateView();
	QGraphicsView::resizeEvent(event);
}

void QuickChatViewWidget::updateView()
{
	if (m_controller) {
		QDeclarativeItem *declarativeItemRoot = m_controller.data()->rootItem();
		if (declarativeItemRoot) {
			if (!qFuzzyCompare(width(), declarativeItemRoot->width()))
				declarativeItemRoot->setWidth(width());
			if (!qFuzzyCompare(height(), declarativeItemRoot->height()))
				declarativeItemRoot->setHeight(height());
			setSceneRect(declarativeItemRoot->boundingRect());
		}
	}
}

void QuickChatViewWidget::onRootChanged(QDeclarativeItem *)
{
	updateView();
}

} // namespace AdiumChat
} // namespace Core


/****************************************************************************
 *  quickchatviewwidget.cpp
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

#include "quickchatviewwidget.h"
#include "quickchatviewcontroller.h"
#include <qutim/config.h>
#include <QDeclarativeItem>
#ifndef QT_NO_OPENGL
#	include <QtOpenGL/QtOpenGL>
#endif

namespace Core {
namespace AdiumChat {

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
	if (m_controller == object)
		return;
	if (m_controller)
		m_controller->disconnect(this);
	QuickChatViewController* controller = qobject_cast<QuickChatViewController*>(object);
	m_controller = controller;
	if (controller) {
		controller->setItemIndexMethod(QGraphicsScene::NoIndex);
		controller->setStickyFocus(true);  //### needed for correct focus handling
		setScene(controller);
		updateView();
		connect(controller, SIGNAL(rootItemChanged()),
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
		QDeclarativeItem *declarativeItemRoot = m_controller->rootItem();
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

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
#include <QVBoxLayout>
#include <QStringBuilder>
#include <QAction>

#include <QDeclarativeView>
#include <QDeclarativeContext>

#include <qutim/config.h>
#include <qutim/thememanager.h>
#include <qutim/debug.h>
#include <qutim/utils.h>
#include <qutim/qtwin.h>

namespace KineticPopups {

using namespace qutim_sdk_0_3;

QuickNotify::QuickNotify(Notification* notify) : 
	QObject(notify),
	m_notify(notify)
{
	foreach (const char *name, notify->dynamicPropertyNames())
		setProperty(name, notify->property(name));

	//add Actions
	foreach (NotificationAction action, notify->request().actions()) {
		QAction *notifyAction = new QAction(action.icon(), action.title(), this);
		connect(notifyAction, SIGNAL(triggered()), SLOT(onActionTriggered()));
		notifyAction->setData(qVariantFromValue(action));
		m_actions.append(notifyAction);
	}
}

void QuickNotify::onActionTriggered()
{
	QAction *action = sender_cast<QAction*>(sender());
	NotificationAction notify = action->data().value<NotificationAction>();
	notify.trigger();
}

QString QuickNotify::text() const
{
	return m_notify->request().text();
}

QString QuickNotify::title() const
{
	QString title = m_notify->request().title();
	if (title.isEmpty())
		title = tr("Notification from qutIM");
	return title;
}

Notification::Type QuickNotify::type() const
{
	return m_notify->request().type();
}

QObject* QuickNotify::object() const
{
	return m_notify->request().object();
}

QObjectList QuickNotify::actions() const
{
	return m_actions;
}

PopupAttributes::PopupAttributes(QObject* parent) : 
	QObject(parent),
	m_frameStyle(Tool)
{
	setObjectName(QLatin1String("attributes"));
}

PopupAttributes::FrameStyle PopupAttributes::frameStyle() const
{
	return m_frameStyle;
}

void PopupAttributes::setFrameStyle(FrameStyle frameStyle)
{
	if (frameStyle == m_frameStyle)
		return;
	m_frameStyle = frameStyle;
	emit frameStyleChanged(m_frameStyle);
}

QuickPopupWidget::QuickPopupWidget(QWidget* parent) :
	PopupWidget(parent),
	m_view(new QDeclarativeView(this))
{
	setAttribute(Qt::WA_DeleteOnClose);
	setAttribute(Qt::WA_ShowWithoutActivating);
	setAttribute(Qt::WA_X11NetWmWindowTypeToolTip);

	//transparency
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TranslucentBackground);
	m_view->viewport()->setAttribute(Qt::WA_TranslucentBackground);
	m_view->viewport()->setAutoFillBackground(false);

	QVBoxLayout *l = new QVBoxLayout(this);
	l->addWidget(m_view);
	l->setMargin(0);
	setLayout(l);
	connect(m_view, SIGNAL(sceneResized(QSize)), SIGNAL(sizeChanged(QSize)));
	m_view->setResizeMode(QDeclarativeView::SizeViewToRootObject);
	m_view->rootContext()->setContextProperty(QLatin1String("popup"), this);

	//TODO optimize!
	Config cfg("behavior");
	cfg.beginGroup("popup");
	loadTheme(cfg.value("themeName", "default"));
	m_timeout.setInterval(cfg.value("timeout", 5000));
	cfg.endGroup();

	connect(&m_timeout, SIGNAL(timeout()), this, SLOT(reject()));
}

void QuickPopupWidget::loadTheme(const QString &themeName)
{
	QString themePath = ThemeManager::path(QLatin1String("quickpopup"), themeName);
	QString filename = themePath % QLatin1Literal("/main.qml");
	m_view->setSource(QUrl::fromLocalFile(filename));//url - main.qml
	if (m_view->status() == QDeclarativeView::Error)
		reject();

	PopupAttributes *attributes = m_view->rootObject()->findChild<PopupAttributes*>("attributes");
	if (attributes) {
		connect(attributes, SIGNAL(frameStyleChanged(KineticPopups::PopupAttributes::FrameStyle)),
				this, SLOT(onAtributesChanged()));
	}
	setPopupAttributes(attributes);

	//reload notifications
	//TODO
}

NotificationList QuickPopupWidget::notifications() const
{
	return m_notifyHash.keys();
}

void QuickPopupWidget::addNotification(Notification* notify)
{
	QuickNotify *quick = new QuickNotify(notify);
	m_notifyHash.insert(notify, quick);
	emit notifyAdded(quick);
	m_timeout.start();
}

QuickPopupWidget::~QuickPopupWidget()
{

}

QSize QuickPopupWidget::sizeHint() const
{
	return m_view->sizeHint();
}

void QuickPopupWidget::ignore()
{
	foreach (Notification *notify, m_notifyHash.keys())
		notify->ignore();
	emit finished();
}

void QuickPopupWidget::accept()
{
	foreach (Notification *notify, m_notifyHash.keys())
		notify->accept();
	emit finished();
}

void QuickPopupWidget::reject()
{
	emit finished();
}

void QuickPopupWidget::onAtributesChanged()
{
	PopupAttributes *attributes = qobject_cast<PopupAttributes*>(sender());
	setPopupAttributes(attributes);
}

void QuickPopupWidget::setPopupAttributes(PopupAttributes *attributes)
{
	PopupAttributes::FrameStyle style = attributes ? attributes->frameStyle() :
													 PopupAttributes::ToolTip;

	Qt::WindowFlags flags = Qt::WindowStaysOnTopHint;
	//set flags
	switch(style) {
	case PopupAttributes::AeroBlur:
		flags |= Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint;
		break;
	case PopupAttributes::ToolTipBlurAero:
		flags |= Qt::ToolTip;
		break;
	case PopupAttributes::ToolTip:
		flags |= Qt::ToolTip | Qt::X11BypassWindowManagerHint | Qt::FramelessWindowHint;
		break;
	case PopupAttributes::ToolBlurAero:
		flags |= Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint
				| Qt::WindowShadeButtonHint;
	case PopupAttributes::Tool:
		flags |= Qt::Tool;
		break;
	case PopupAttributes::Normal:
		break;
	}
	setWindowFlags(flags);

	//enable aero
	switch(style) {
	case PopupAttributes::ToolBlurAero:
	case PopupAttributes::AeroBlur:
		QtWin::extendFrameIntoClientArea(this);
		break;
	case PopupAttributes::ToolTipBlurAero:
		QtWin::enableBlurBehindWindow(this, true);
		break;
	default:
		break;
	}

	emit sizeChanged(sizeHint());
}

void QuickPopupWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
		accept();
	else if (event->button() == Qt::RightButton)
		ignore();
}

} // namespace KineticPopups

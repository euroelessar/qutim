/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "adiumwebviewwidget.h"
#include "adiumwebviewcontroller.h"
#include <qutim/config.h>
#include <qutim/servicemanager.h>
#include <QWebFrame>
#include <QApplication>
#ifdef Q_WS_MAEMO_5
#include <QMouseEvent>
#endif

namespace Adium {

using namespace qutim_sdk_0_3;

WebViewWidget::WebViewWidget(QWidget *parent)
	: QWebView(parent)
{
	if (QObject *scroller = ServiceManager::getByName("Scroller"))
		QMetaObject::invokeMethod(scroller,
								  "enableScrolling",
								  Q_ARG(QObject*, this));
#ifdef Q_WS_MAEMO_5
	mousePressed = false;
	installEventFilter(this);
#endif
}

void WebViewWidget::setViewController(QObject* object)
{
	if (m_controller.data() == object)
		return;

	if (!m_controller.isNull()) {
		m_controller.data()->setView(0);
		m_controller.data()->disconnect(this);
	}

	WebViewController *controller = qobject_cast<WebViewController*>(object);
	m_controller = controller;
	if (controller)
		setPage(controller);
	else
		setPage(new QWebPage(this));
}

#ifdef Q_WS_MAEMO_5
bool WebViewWidget::eventFilter(QObject *, QEvent *e)
{
    switch (e->type()) {
    case QEvent::MouseButtonPress:
	if (static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton)
	    mousePressed = true;
	break;
    case QEvent::MouseButtonRelease:
	if (static_cast<QMouseEvent *>(e)->button() == Qt::LeftButton)
	    mousePressed = false;
	break;
    case QEvent::MouseMove:
	if (mousePressed)
	    return true;
	break;
    default:
	break;
    }
    return false;
}
#endif

} // namespace Adium


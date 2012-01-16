/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

namespace Adium {

WebViewWidget::WebViewWidget(QWidget *parent)
	: QWebView(parent)
{
//	viewport()->setFocusPolicy(Qt::NoFocus);
//	setFocusPolicy(Qt::StrongFocus);
}

void WebViewWidget::setViewController(QObject* object)
{
	if (m_controller == object)
		return;
	if (m_controller)
		m_controller->disconnect(this);
	WebViewController *controller = qobject_cast<WebViewController*>(object);
	m_controller = controller;
	if (controller) {
		setPage(controller);
	} else {
		setPage(new QWebPage(this));
	}
}

} // namespace Adium


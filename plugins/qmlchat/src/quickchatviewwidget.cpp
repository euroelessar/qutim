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
#include <QQuickItem>
#include <QApplication>

namespace Core {
namespace AdiumChat {

using namespace qutim_sdk_0_3;

QuickChatViewWidget::QuickChatViewWidget()
{
    setResizeMode(QQuickView::SizeRootObjectToView);
    setColor(qApp->palette().window().color());
}

void QuickChatViewWidget::setViewController(QObject* object)
{
	if (m_controller.data() == object)
		return;
	if (m_controller)
		m_controller.data()->disconnect(this);
	m_controller = qobject_cast<QuickChatController*>(object);
}

void QuickChatViewWidget::onRootChanged(QQuickItem *item)
{
}

} // namespace AdiumChat
} // namespace Core


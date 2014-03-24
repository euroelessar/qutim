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
#include <qutim/systeminfo.h>
#include <QQuickItem>
#include <QApplication>
#include <QVBoxLayout>

namespace Core {
namespace AdiumChat {

using namespace qutim_sdk_0_3;

QuickChatViewWidget::QuickChatViewWidget()
{
    m_container = QWidget::createWindowContainer(&m_view, this);
    auto layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_container);

    m_view.setResizeMode(QQuickView::SizeRootObjectToView);
    m_view.setColor(qApp->palette().window().color());

    QString path = SystemInfo::getPath(SystemInfo::SystemShareDir);
    path += QStringLiteral("/qml/qmlchat/ChatView.qml");

    m_view.setSource(path);

    qDebug() << "chat" << m_view.rootObject();
    if (!m_view.rootObject())
        qWarning() << m_view.errors();
    Q_ASSERT(m_view.rootObject());
}

QuickChatViewWidget::~QuickChatViewWidget()
{
    if (m_connection)
        QObject::disconnect(m_connection);
}

void QuickChatViewWidget::setViewController(QObject* object)
{
	if (m_controller == object)
		return;

    if (m_connection)
        QObject::disconnect(m_connection);

	m_controller = qobject_cast<QuickChatController*>(object);
    auto onItemChanged = [this] (QQuickItem *item) {
        bool result = QMetaObject::invokeMethod(m_view.rootObject(), "setItem", Q_ARG(QVariant, QVariant::fromValue(item)));
        (void) result;
        Q_ASSERT(result);
    };

    if (m_controller) {
        onItemChanged(m_controller->item());
        m_connection = connect(m_controller.data(), &QuickChatController::itemChanged, onItemChanged);
    } else {
        onItemChanged(NULL);
    }
}

} // namespace AdiumChat
} // namespace Core


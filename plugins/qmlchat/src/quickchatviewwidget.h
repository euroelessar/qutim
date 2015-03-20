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

#ifndef QUICKCHATVIEWWIDGET_H
#define QUICKCHATVIEWWIDGET_H
#include <QGraphicsView>
#include <qutim/adiumchat/chatviewfactory.h>
#include <qutim/declarativeview.h>
#include <QPointer>

class QDeclarativeItem;
namespace Core {
namespace AdiumChat {

class QuickChatController;
class QuickChatViewWidget : public QWidget, public Core::AdiumChat::ChatViewWidget
{
	Q_OBJECT
	Q_INTERFACES(Core::AdiumChat::ChatViewWidget)
public:
	QuickChatViewWidget();
    ~QuickChatViewWidget();

    virtual void setViewController(QObject* controller);
private:
    qutim_sdk_0_3::DeclarativeView m_view;
    QWidget *m_container;
	QPointer<QuickChatController> m_controller;
    QMetaObject::Connection m_connection;
};

} // namespace AdiumChat
} // namespace Core

#endif // QUICKCHATVIEWWIDGET_H


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

#ifndef CORE_ADIUMCHAT_QUOTERWIDGET_H
#define CORE_ADIUMCHAT_QUOTERWIDGET_H

#include <QListWidget>
#include <QTimer>
#include <qutim/message.h>
#include <QPointer>

namespace Core {
namespace AdiumChat {

class QuoterWidget : public QListWidget
{
	Q_OBJECT
public:
	QuoterWidget(const qutim_sdk_0_3::MessageList &messages, QObject *controller);
	~QuoterWidget();
	
signals:
	void quoteChoosed(const QString &quote, QObject *controller);
	
public slots:
	void nextRow();
	
private:
	QTimer m_timer;
	QPointer<QObject> m_controller;
};

} // namespace AdiumChat
} // namespace Core

#endif // CORE_ADIUMCHAT_QUOTERWIDGET_H

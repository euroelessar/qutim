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

#ifndef JAVASCRIPTCLIENT_H
#define JAVASCRIPTCLIENT_H

#include <QObject>
namespace Core
{
namespace AdiumChat
{
class ChatSessionImpl;

class JavaScriptClient : public QObject
{
	Q_OBJECT
public:
	JavaScriptClient(ChatSessionImpl *session);

public slots:
	void debugLog(const QVariant &text);
	bool zoomImage(const QVariant &text);
	void helperCleared();
	void appendNick(const QVariant &nick);
	void contextMenu(const QVariant &nickVar);
	void appendText(const QVariant &text);

private:
	ChatSessionImpl *m_session;
};
}
}

#endif // JAVASCRIPTCLIENT_H


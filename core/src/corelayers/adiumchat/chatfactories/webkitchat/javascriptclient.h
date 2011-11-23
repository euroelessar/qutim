/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include <QWebFrame>

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
	
public:
	void setStylesheet(const QString &id, const QString &url);
	void setCustomStylesheet(const QString &url);
	void addSeparator();
	void appendMessage(const QString &text);
	void appendNextMessage(const QString &text);
	void setupScripts(QWebFrame *frame);

public slots:
	void debugLog(const QVariant &text);
	void debugLog();
	bool zoomImage(const QVariant &text);
	void appendNick(const QVariant &nick);
	void contextMenu(const QVariant &nickVar);
	void appendText(const QVariant &text);
	
private slots:
	void helperCleared();
	void onLoadFinished();

signals:
	void setStylesheetRequest(const QString &id, const QString &url);
	void setCustomStylesheetRequest(const QString &url);
	void addSeparatorRequest();
	void appendMessageRequest(const QString &text);
	void appendNextMessageRequest(const QString &text);
	void someSignal();
	
protected:
    void connectNotify(const char *signal);
    void disconnectNotify(const char *signal);

private:
	ChatSessionImpl *m_session;
};
}
}

#endif // JAVASCRIPTCLIENT_H


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

#include "javascriptclient.h"
#include "chatsessionimpl.h"
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QWebElement>
#include <qutim/servicemanager.h>
#include <QFile>

namespace Core
{
namespace AdiumChat
{

JavaScriptClient::JavaScriptClient(ChatSessionImpl *session) :
	QObject(session),
	m_session(session),
	m_isLoading(false)
{
	setObjectName(QLatin1String("client"));
}

void JavaScriptClient::setStylesheet(const QString &id, const QString &url)
{
	if (m_isLoading) {
		PostMessage msg = {SetStylesheet, url, id};
		m_messages.append(msg);
	} else
		emit setStylesheetRequest(id, url);
}

void JavaScriptClient::setCustomStylesheet(const QString &url)
{
	if (m_isLoading) {
		PostMessage msg = {SetCustomStylesheet, url, QString()};
		m_messages.append(msg);
	} else
		emit setCustomStylesheetRequest(url);
}

void JavaScriptClient::addSeparator()
{
	if (m_isLoading) {
		PostMessage msg = {AddSeparator, QString(), QString()};
		m_messages.append(msg);
	} else
		emit addSeparatorRequest();
}

void JavaScriptClient::appendMessage(const QString &text)
{
	if (m_isLoading) {
		PostMessage msg = {AppendMessage, text, QString()};
		m_messages.append(msg);
	} else
		emit appendMessageRequest(text);
}

void JavaScriptClient::appendNextMessage(const QString &text)
{
	if (m_isLoading) {
		PostMessage msg = {AppendNextMessage, text, QString()};
		m_messages.append(msg);
	} else
		emit appendNextMessageRequest(text);
}

void JavaScriptClient::setupScripts(QWebFrame *frame)
{
	frame->addToJavaScriptWindowObject(objectName(), this);
	QString path = QLatin1String(":/share/signals.js");
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly))
		return;
	QString script = file.readAll();
	frame->evaluateJavaScript(script);
}

void JavaScriptClient::setWebFrame(QWebFrame *frame)
{
	if (m_webFrame)
		m_webFrame.data()->disconnect(this);

	m_webFrame = frame;
	connect(frame, SIGNAL(loadStarted()), SLOT(onLoadStarted()));
	connect(frame, SIGNAL(loadFinished(bool)), SLOT(onLoadFinished()));
	connect(frame, SIGNAL(javaScriptWindowObjectCleared()), SLOT(helperCleared()));
}

void JavaScriptClient::postMessages()
{
	foreach (PostMessage msg, m_messages) {
		switch (msg.type) {
		case AppendMessage:
			emit appendMessage(msg.text);
			break;
		case AppendNextMessage:
			emit appendNextMessage(msg.text);
			break;
		case AddSeparator:
			emit addSeparatorRequest();
			break;
		case SetStylesheet:
			emit setStylesheetRequest(msg.id, msg.text);
			break;
		case SetCustomStylesheet:
			emit setCustomStylesheetRequest(msg.text);
			break;
		default:
			break;
		}
	}
	m_messages.clear();
	//qDebug() << m_webFrame.data()->toHtml();
}

void JavaScriptClient::debug(const QVariant &text)
{
	qDebug("WebKit: \"%s\"", qPrintable(text.toString()));
}

void JavaScriptClient::debug()
{
	qDebug("WebKit: Unknown message");
}

bool JavaScriptClient::zoomImage(const QVariant &)
{
	//TODO WTF? Oo
	return false;
}

void JavaScriptClient::helperCleared()
{
	//	qDebug("%s", Q_FUNC_INFO);
	if(QWebFrame *frame = qobject_cast<QWebFrame *>(sender())) {
		//qDebug() << Q_FUNC_INFO << frame << frame->toHtml().size();
		setupScripts(frame);
		//frame->addToJavaScriptWindowObject(objectName(), this);
	}
}

void JavaScriptClient::onLoadFinished()
{
	QWebFrame *frame = static_cast<QWebFrame*>(sender());
	setupScripts(frame);
	m_isLoading = false;
	postMessages();
}

void JavaScriptClient::onLoadStarted()
{
	m_isLoading = true;
}

void JavaScriptClient::appendNick(const QVariant &nick)
{
	QObject *form = ServiceManager::getByName("ChatForm");
	QObject *obj = 0;
	if (QMetaObject::invokeMethod(form, "textEdit", Q_RETURN_ARG(QObject*, obj),
								  Q_ARG(qutim_sdk_0_3::ChatSession*, m_session)) && obj) {
		QTextCursor cursor;
		if (QTextEdit *edit = qobject_cast<QTextEdit*>(obj))
			cursor = edit->textCursor();
		else if (QPlainTextEdit *edit = qobject_cast<QPlainTextEdit*>(obj))
			cursor = edit->textCursor();
		else
			return;
		if(cursor.atStart())
			cursor.insertText(nick.toString() + ": ");
		else
			cursor.insertText(nick.toString() + " ");
		static_cast<QWidget*>(obj)->setFocus();
	}
}

void JavaScriptClient::contextMenu(const QVariant &nickVar)
{
	QString nick = nickVar.toString();
	foreach (ChatUnit *unit, m_session->getUnit()->lowerUnits()) {
		if (Buddy *buddy = qobject_cast<Buddy*>(unit)) {
			if (buddy->name() == nick)
				buddy->showMenu(QCursor::pos());
		}
	}
}

void JavaScriptClient::appendText(const QVariant &text)
{
	QObject *form = ServiceManager::getByName("ChatForm");
	QObject *obj = 0;
	if (QMetaObject::invokeMethod(form, "textEdit", Q_RETURN_ARG(QObject*, obj),
								  Q_ARG(qutim_sdk_0_3::ChatSession*, m_session)) && obj) {
		QTextCursor cursor;
		if (QTextEdit *edit = qobject_cast<QTextEdit*>(obj))
			cursor = edit->textCursor();
		else if (QPlainTextEdit *edit = qobject_cast<QPlainTextEdit*>(obj))
			cursor = edit->textCursor();
		else
			return;
		cursor.insertText(text.toString());
		cursor.insertText(" ");
		static_cast<QWidget*>(obj)->setFocus();
	}
}

void JavaScriptClient::connectNotify(const char *signal)
{
	qDebug() << Q_FUNC_INFO << signal;
}

void JavaScriptClient::disconnectNotify(const char *signal)
{
	qDebug() << Q_FUNC_INFO << signal;
}
}

}


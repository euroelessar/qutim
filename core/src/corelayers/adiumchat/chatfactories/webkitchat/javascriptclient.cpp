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
	m_session(session)
{
	setObjectName(QLatin1String("client"));
}

void JavaScriptClient::setStylesheet(const QString &id, const QString &url)
{
	emit setStylesheetRequest(id, url);
}

void JavaScriptClient::setCustomStylesheet(const QString &url)
{
	emit setCustomStylesheetRequest(url);
}

void JavaScriptClient::addSeparator()
{
	emit addSeparatorRequest();
}

void JavaScriptClient::appendMessage(const QString &text)
{
	emit appendMessageRequest(text);
}

void JavaScriptClient::appendNextMessage(const QString &text)
{
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


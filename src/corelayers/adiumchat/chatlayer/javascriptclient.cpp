/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#include "javascriptclient.h"
#include "chatsessionimpl.h"
#include <QWebFrame>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <qutim/servicemanager.h>

namespace Core
{
namespace AdiumChat
{
JavaScriptClient::JavaScriptClient(ChatSessionImpl *session) :
	QObject(session)
{
	setObjectName(QLatin1String("client"));
	m_session = session;
}

void JavaScriptClient::debugLog(const QVariant &text)
{
	qDebug("WebKit: \"%s\"", qPrintable(text.toString()));
}

bool JavaScriptClient::zoomImage(const QVariant &text)
{
	return false;
}

void JavaScriptClient::helperCleared()
{
	if(QWebFrame *frame = qobject_cast<QWebFrame *>(sender()))
		frame->addToJavaScriptWindowObject(objectName(), this);
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
}
}

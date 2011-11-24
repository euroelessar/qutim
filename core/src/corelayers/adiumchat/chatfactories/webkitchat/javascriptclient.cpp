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

void JavaScriptClient::setStylesheet(const QString &id, const QString &url)
{
	qDebug() << Q_FUNC_INFO << id << url;
	emit setStylesheetRequest(id, url);
}

void JavaScriptClient::setCustomStylesheet(const QString &url)
{
	qDebug() << Q_FUNC_INFO << url;
	emit setCustomStylesheetRequest(url);
}

void JavaScriptClient::addSeparator()
{
	qDebug() << Q_FUNC_INFO;
	emit addSeparatorRequest();
}

void JavaScriptClient::appendMessage(const QString &text)
{
	qDebug() << Q_FUNC_INFO << text;
	qDebug() << objectName() << this;
//	emit messageAppended("Test message");
	emit appendMessageRequest(text);
}

void JavaScriptClient::appendNextMessage(const QString &text)
{
	qDebug() << Q_FUNC_INFO << text;
	qDebug() << objectName() << this;
//	emit messageAppended("Test message");
	emit appendNextMessageRequest(text);
}

void JavaScriptClient::setupScripts(QWebFrame *frame)
{
	qDebug("%s", Q_FUNC_INFO);
	frame->addToJavaScriptWindowObject(objectName(), this);
//	"client.debugLog('Is it work?!');\n"
//    "(function() {\n"
//    "	var object = window.client;\n"
//    "	client.stylesheetSet.connect(setStylesheet);\n"
//    "	client.customStylesheetSet.connect(setCustomStylesheet);\n"
//    "	client.messageAppended.connect(appendMessage);" 
//    "		[ client.nextMessageAppended, function(text) {\n"
//    "		client.debugLog('appendNextMessage ' + text.length);\n"
//    "		try {\n"
//    "			appendNextMessage(text);\n"
//    "		} catch (e) {\n"
//    "			client.debugLog(e.toString());"
//    "		}\n"
//    "	});\n"
//    "		[ client.separatorRequested, function() {\n"
//    "		var separator = document.getElementById(\"separator\");\n"
//    "		if (separator)\n"
//    "			separator.parentNode.removeChild(separator);\n"
//    "		appendMessage(\"<hr id='separator'><div id='insert'></div>\");\n"
//    "	});\n"
//    "	client.debugLog(appendMessage.toString());\n"
//    "	client.debugLog(client.objectName);\n"
//    "	client.debugLog(object === undefined ? 'undefined' : client.toString());\n"
//    "		[ client.someSignal, function() { client.debugLog('Some signal execution'); });\n"
//    "	return true;"
//    "})();\n"
//	QString script =
//	        QLatin1String(
//	            "(function() {\n"
//	            "try {\n"
//	            "client.debugLog('Is it work?! ' + (window.__qutimScriptClientHook === undefined).toString() + ' ' + (window.appendMessage === undefined).toString());\n"
//	            "if (window.__qutimScriptClientHook !== undefined) {\n"
//	            "	if (window.__qutimScriptClientHook.isValid())\n"
//	            "		return false;\n"
//	            "	else\n"
//	            "		window.__qutimScriptClientHook.clear();\n"
//	            "}\n"
//	            "window.__qutimScriptClientHook = {\n"
//	            "	\"isValid\": function() { try { return appendMessage !== undefined; } catch (e) { client.debugLog('isValid ' + e.toString); return false; } },\n"
//	            "	\"connections\": [\n"
//				"		function(id, url) {\n"
//				"			client.debugLog('setStylesheet ' + (appendMessage === undefined).toString());\n"
//				"			if (window.__qutimScriptClientHook.isValid())\n"
//				"				setStylesheet(id, url);\n"
//				"		},\n"
//				"		function(url) {\n"
//				"			client.debugLog('setCustomStylesheet ' + (appendMessage === undefined).toString());\n"
//				"			if (window.__qutimScriptClientHook.isValid())\n"
//				"				setCustomStylesheet(url);\n"
//				"		},\n"
//				"		function(text) {\n"
//				"			client.debugLog('appendMessage ' + (appendMessage === undefined).toString());\n"
//				"			if (window.__qutimScriptClientHook.isValid())\n"
//				"				appendMessage(text);\n"
//				"		},\n"
//	            "		function(text) {\n"
//	            "			client.debugLog('appendNextMessage ' + (appendMessage === undefined).toString());\n"
//				"			if (window.__qutimScriptClientHook.isValid())\n"
//	            "				appendNextMessage(text);\n"
//				"		},\n"
//	            "		function() {\n"
//	            "			client.debugLog('separator ' + (appendMessage === undefined).toString());\n"
//				"			if (window.__qutimScriptClientHook.isValid()) {\n"
//	            "				var separator = document.getElementById(\"separator\");\n"
//	            "				if (separator)\n"
//	            "					separator.parentNode.removeChild(separator);\n"
//				"				appendMessage(\"<hr id='separator'><div id='insert'></div>\");\n"
//				"			}\n"
//				"		},\n"
//	            "		function() {\n"
//	            "			try { client.debugLog('Some signal execution ' + (appendMessage === undefined).toString()); } catch (e) { client.debugLog('someSignal ' + e.toString()); }; \n"
//				"		}\n"
//	            "	],\n"
//	            "	\"getSignals\": function() {\n"
//	            "		return [\n"
//	            "			client.stylesheetSet, client.customStylesheetSet, client.messageAppended,\n"
//	            "			client.nextMessageAppended, client.separatorRequested, client.someSignal\n"
//	            "		];"
//	            "	},\n"
//	            "	\"init\": function() {\n"
//	            "		var signals = window.__qutimScriptClientHook.getSignals();\n"
//	            "		var connections = window.__qutimScriptClientHook.connections;\n"
//	            "		client.debugLog('init ' + signals.length);"
//	            "		for (var i = 0; i < connections.length; ++i)\n"
//	            "			signals[i].connect(connections[i]);\n"
//	            "		client.debugLog('inited ' + connections.length);"
//	            "	},\n"
//	            "	\"clear\": function() {\n"
//	            "		var signals = window.__qutimScriptClientHook.getSignals();\n"
//	            "		var connections = window.__qutimScriptClientHook.connections;\n"
//	            "		client.debugLog('clear ' + signals.length);"
//	            "		for (var i = 0; i < connections.length; ++i)\n"
//	            "			signals[i].disconnect(connections[i]);\n"
//	            "		client.debugLog('cleared ' + connections.length);"
//	            "	}\n"
//	            "};\n"
//	            "window.__qutimScriptClientHook.init();\n"
//	            "} catch (e) {\n"
//	            "	client.debugLog(e.toString());\n"
//	            "	return 'Exception! ' + e.toString();\n"
//	            "}\n"
//	            "return true;\n"
//	            "})();\n"
//				);
//	qDebug() << script;
	QString script = QLatin1String(
	            "(function() {\n"
	            "try {\n"
//	            "	client.debugLog(setStylesheet.toString());\n"
	            "	window['setStylesheet'] = setStylesheet;\n"
//	            "	client.debugLog(window['setStylesheet'].toString());\n"
	            "	window['setCustomStylesheet'] = setCustomStylesheet;\n"
	            "	window['appendMessage'] = appendMessage;\n"
	            "	window['appendNextMessage'] = appendNextMessage;\n"
	            "	window['addSeparator'] = function() {\n"
				"		var separator = document.getElementById(\"separator\");\n"
				"		if (separator)\n"
				"			separator.parentNode.removeChild(separator);\n"
				"		window['appendMessage'](\"<hr id='separator'><div id='insert'></div>\");\n"
	            "	};\n"
	            "	if (window['clientConnector'] !== undefined)\n"
	            "		return false;\n"
	            "	client.debugLog('He? Let\\\'s try!');\n"
	            "	var connector = {\n"
	            "		\"methods\": [\n"
	            "			'setStylesheet',\n"
	            "			'setCustomStylesheet',\n"
	            "			'appendMessage',\n"
	            "			'appendNextMessage',\n"
	            "			'addSeparator'\n"
	            "		],\n"
	            "		\"init\": function() {\n"
	            "			var methods = connector.methods;\n"
	            "			for (var i = 0; i < methods.length; ++i) {\n"
	            "				var signal = methods[i] + 'Request';\n"
	            "				var method = methods[i];\n"
	            "				var object = { id: method };\n"
	            "				client.debugLog('Trying to connect ' + signal + ' to ' + method);\n"
//	            "				client.debugLog(window[method].toString());\n"
	            "				client[signal].connect(object, function () {\n"
	            "					try {\n"
	            "						var method = this.id;\n"
	            "						if (window[method] !== undefined)\n"
	            "							window[method](arguments[0], arguments[1]);\n"
	            "					} catch (e) {\n"
	            "						client.debugLog(JSON.stringify(e));"
	            "					}\n"
				"				});\n"
	            "			}\n"
	            "		}\n"
	            "	};\n"
	            "	window.clientConnector = connector;\n"
	            "	connector.init();\n"
	            "	return true;\n"
	            "} catch (e) {\n"
	            "	return 'Exception: ' + e.toString();\n"
	            "}\n"
	            "})();\n"
	            );
//	qDebug() << script;
//	qDebug() << frame->evaluateJavaScript("client.debugLog('Hello!');");
	qDebug() << frame->evaluateJavaScript(script);
//	QString tag = "<script type=\"text/ecmascript\" defer=\"defer\">\n" + script + "\n</script>";
//	QWebElement element = frame->findFirstElement("script");
//	QWebElement element2 = element.clone();
//	element2.setInnerXml(script);
//	frame->findFirstElement("head").appendInside(element2);
//	frame->evaluateJavaScript(script);
//	qDebug() << frame->toHtml();
//	emit messageAppended("Test message");
	qDebug() << objectName() << this;
	emit someSignal();
	emit someSignal();
	emit someSignal();
}

void JavaScriptClient::debugLog(const QVariant &text)
{
	qDebug("WebKit: \"%s\"", qPrintable(text.toString()));
}

void JavaScriptClient::debugLog()
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
		qDebug() << Q_FUNC_INFO << frame << frame->toHtml().size();
//		setupScripts(frame);
		frame->addToJavaScriptWindowObject(objectName(), this);
////		setupScripts(frame);
	}
}

void JavaScriptClient::onLoadFinished()
{
	QWebFrame *frame = qobject_cast<QWebFrame *>(sender());
	qDebug() << Q_FUNC_INFO << frame << frame->toHtml().size();
//	frame->addToJavaScriptWindowObject(objectName(), this);
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


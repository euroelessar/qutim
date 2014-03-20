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
#include "chatedit.h"
#include "chatsessionimpl.h"
#include <QDateTime>
#include <QTextBlock>
#include <QKeyEvent>
#include <QTextDocumentFragment>
#include <qutim/notification.h>
#include <qutim/debug.h>
#include <qutim/messagehandler.h>
#include <QMetaProperty>
#include <qutim/config.h>
#include <QApplication>

namespace Core
{
namespace AdiumChat
{

QString ChatEdit::textEditToPlainText()
{
	QTextDocument *doc = document();
	QString result;
	result.reserve(doc->characterCount());
	QTextCursor begin(doc);
	QTextCursor end;
	QString specialChar = QChar(QChar::ObjectReplacementCharacter);
	bool first = true;
	while (!begin.atEnd()) {
		end = doc->find(specialChar, begin, QTextDocument::FindCaseSensitively);
		QString postValue;
		bool atEnd = end.isNull();
		if (atEnd) {
			end = QTextCursor(doc);
			QTextBlock block = doc->lastBlock();
			end.setPosition(block.position() + block.length() - 1);
		} else {
			postValue = end.charFormat().toolTip();
		}
		begin.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor,
						   end.position() - begin.position() - (atEnd ? 0 : 1));
		QString selectionText = begin.selection().toPlainText();
		if (!first)
			result += selectionText.midRef(1);
		else
			result += selectionText;
		result += postValue;
		begin = end;
		end.clearSelection();
		first = false;
	}
	return result;
}

ChatEdit::ChatEdit(QWidget *parent) :
	QTextEdit(parent),
	m_sendKey(SendCtrlEnter)
{
	setMinimumHeight(QFontMetrics(font()).height() * 2);
	setAcceptRichText(false);
	m_autoResize = false;
	connect(this,SIGNAL(textChanged()),SLOT(onTextChanged()));
}

void ChatEdit::setSession(ChatSessionImpl *session)
{
	m_session = session;
	setDocument(session->getInputField());
	setFocus();
	QFont chatEditFont = qApp->font();
	Config cfg = Config("appearance");
	cfg.beginGroup("chat");
	chatEditFont.setPointSize(cfg.value("chatFontSize", qApp->font().pointSize()));
	cfg.endGroup();
	session->getInputField()->setDefaultFont(chatEditFont);
}

bool ChatEdit::event(QEvent *event)
{
	if (event->type() == QEvent::KeyPress) {
		QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
		if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
			if (keyEvent->modifiers() == Qt::ControlModifier)  {
				if (m_sendKey == SendCtrlEnter) {
					send();
					return true;
				} else if (m_sendKey == SendEnter || m_sendKey == SendDoubleEnter) {
					insertPlainText(QLatin1String("\n"));
				}
			} else if (keyEvent->modifiers() == Qt::NoModifier
					   || keyEvent->modifiers() == Qt::KeypadModifier) {
				if (m_sendKey == SendEnter) {
					send();
					return true;
				} else if (m_sendKey == SendDoubleEnter) {
					m_entersCount++;
					if (m_entersCount > 1) {
						m_enterPosition.deletePreviousChar();
						m_entersCount = 0;
						send();
						return true;
					} else {
						m_enterPosition = textCursor();
					}
				}
			}
		} else {
			m_entersCount = 0;
		}
	}
	return QTextEdit::event(event);
}

class ChatEditLocker
{
public:
	ChatEditLocker(QWidget *widget) : m_widget(widget)
	{
		m_focus = widget->hasFocus();
		m_widget->setDisabled(true);
	}

    ChatEditLocker(ChatEditLocker &&other) : m_widget(other.m_widget), m_focus(other.m_focus)
    {
        other.m_widget.clear();
    }

	~ChatEditLocker()
	{
        if (m_widget) {
            m_widget->setDisabled(false);
            if (m_focus)
                m_widget->setFocus();
        }
	}

private:
    QPointer<QWidget> m_widget;
	bool m_focus;
};

void ChatEdit::send()
{
	if (!isEnabled())
		return;

	ChatEditLocker locker(this);

	QString text = textEditToPlainText();
	QString trimmed = text.trimmed();
	if(!m_session || trimmed.isEmpty())
		return;
	
	ChatUnit *unit = m_session.data()->getCurrentUnit();
	if (trimmed.startsWith(QLatin1Char('/')) && trimmed.size() > 1) {
		int index = trimmed.indexOf(QLatin1Char(' '));
		QStringRef cmd = trimmed.midRef(1, (index == -1 ? trimmed.size() : index) - 1);
		const QMetaObject *meta = unit->metaObject();
		for (int i = meta->propertyCount() - 1; i >= 0; --i) {
			QMetaProperty prop = meta->property(i);
			if (prop.isWritable() && QLatin1String(prop.name()) == cmd) {
				prop.write(unit, trimmed.mid(index + 1));
				clear();
				return;
			}
		}
		QByteArray signature = cmd.toString().toLatin1();
		int methodIndex = meta->indexOfMethod(signature + "(QString)");
		if (methodIndex != -1) {
			QMetaMethod method = meta->method(methodIndex);
			QString arg = trimmed.mid(index + 1);
			if (method.invoke(unit, Q_ARG(QString, arg))) {
				clear();
				return;
			}
		}
		methodIndex = meta->indexOfMethod(signature + "(QVariant)");
		if (methodIndex != -1) {
			QMetaMethod method = meta->method(methodIndex);
			QVariant arg = trimmed.mid(index + 1);
			if (method.invoke(unit, Q_ARG(QVariant, arg))) {
				clear();
				return;
			}
		}
		methodIndex = meta->indexOfMethod(signature + "()");
		if (methodIndex != -1) {
			QMetaMethod method = meta->method(methodIndex);
			if (method.invoke(unit)) {
				clear();
				return;
			}
		}
	}
	Message message(text);
	message.setIncoming(false);
	message.setChatUnit(unit);
	message.setTime(QDateTime::currentDateTime());
	MessageHandler::traceHandlers();

    auto lockerPtr = QSharedPointer<ChatEditLocker>::create(std::move(locker));
    QPointer<ChatEdit> edit(this);
    m_session->append(message, [edit, lockerPtr] (qint64 result, const Message &, const QString &) mutable {
        if (MessageHandler::Error != -result && edit)
            edit->clear();
        lockerPtr.reset();
    });
}

void ChatEdit::onTextChanged()
{
	if(!m_session)
		return;
	if(m_autoResize) {
		QFontMetrics fontHeight = fontMetrics();
		//const int docHeight = document()->size().toSize().height()*fontHeight.height() + int(document()->documentMargin()) * 3;
		const int docHeight = document()->size().toSize().height()+int(document()->documentMargin());
//		qDebug() << "New docHeight is: " << docHeight;
		if (docHeight == previousTextHeight)
			return;

		previousTextHeight = docHeight;
		const int resHeight = qMin(window()->height() / 3, qMax(docHeight, fontHeight.height()));
		setMinimumHeight(resHeight);
		setMaximumHeight(resHeight);
	}

	QString text = textEditToPlainText();
	if(!m_session || text.trimmed().isEmpty())
		m_session.data()->setChatState(ChatUnit::ChatStateActive);
	else
		m_session.data()->setChatState(ChatUnit::ChatStateComposing);
}

void ChatEdit::setSendKey(SendMessageKey key)
{
	m_sendKey = key;
}

void ChatEdit::setAutoResize(bool resize)
{
	if(!resize)
		setMaximumHeight(QWIDGETSIZE_MAX);
	m_autoResize = resize;
}

}
}


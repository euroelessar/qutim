#include "chatedit.h"
#include "chatsessionimpl.h"
#include <QDateTime>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <qutim/notification.h>
#include <qutim/debug.h>
#include <qutim/messagehandler.h>
#include <QMetaProperty>

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
	QPlainTextEdit(parent),
	m_session(0),
	m_sendKey(SendCtrlEnter)
{
	setMinimumHeight(QFontMetrics(font()).height()*2);
	connect(this,SIGNAL(textChanged()),SLOT(onTextChanged()));
}

void ChatEdit::setSession(ChatSessionImpl *session)
{
	m_session = session;
	setDocument(session->getInputField());
	setFocus();
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
	return QPlainTextEdit::event(event);
}

void ChatEdit::send()
{
	QString text = textEditToPlainText();
	QString trimmed = text.trimmed();
	if(!m_session || trimmed.isEmpty())
		return;
	
	ChatUnit *unit = m_session->getCurrentUnit();
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

	qint64 result = m_session->appendMessage(message);
	if (MessageHandler::Error != -result)
		clear();
//	if (!unit->sendMessage(message)) {
//		NotificationRequest request(Notification::System);
//		request.setObject(this);
//		request.setText(tr("Unable to send message to %1").arg(unit->title()));
//		request.send();
//	}
//	else {
//		m_session->appendMessage(message);
//		clear();
//	}
}

void ChatEdit::onTextChanged()
{
	if(!m_session)
		return;

	QString text = textEditToPlainText();
	if(!m_session || text.trimmed().isEmpty())
		m_session->setChatState(ChatStateActive);
	else
		m_session->setChatState(ChatStateComposing);
}

void ChatEdit::setSendKey(SendMessageKey key)
{
	m_sendKey = key;
}

}
}

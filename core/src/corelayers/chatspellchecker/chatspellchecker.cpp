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

#include "chatspellchecker.h"
#include <qutim/servicemanager.h>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QContextMenuEvent>

namespace Core {

SpellHighlighter::SpellHighlighter(QTextDocument *doc) : QSyntaxHighlighter(doc)
{
	m_format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
	m_format.setUnderlineColor(Qt::red);
}

void SpellHighlighter::highlightBlock(const QString &text)
{
	if (!m_speller)
		return;

	static QRegExp expression(QLatin1String("\\b\\w+\\b"));

	int index = text.indexOf(expression);
	while (index >= 0) {
		int length = expression.matchedLength();
		if (!m_speller->isCorrect(expression.cap(0)))
			setFormat(index, length, m_format);
		index = text.indexOf(expression, index + length);
	}
}

ChatSpellChecker::ChatSpellChecker() : m_chatForm("ChatForm")
{
	if (m_speller)
		connect(m_speller, SIGNAL(dictionaryChanged()), SLOT(onDictionaryChanged()));
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
	connect(ServiceManager::instance(), SIGNAL(serviceChanged(QByteArray,QObject*,QObject*)),
			SLOT(onServiceChanged(QByteArray)));
}

void ChatSpellChecker::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	Q_ASSERT(session);
	QTextDocument *inputField = session->getInputField();
	if (inputField) {
		SpellHighlighter *highlighter = new SpellHighlighter(inputField);
		if (m_chatForm) {
			connect(session, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
			// We use m_highlighter only in onAddToDictionaryTriggered
			// which run only when ChatForm service is enabled (i.e., m_chatForm != 0).
			m_highlighters.insert(inputField, highlighter);
			connect(inputField, SIGNAL(destroyed(QObject*)), SLOT(onInputFieldDestroyed(QObject*)));
		}
	}
}

void ChatSpellChecker::onInputFieldDestroyed(QObject *obj)
{
	m_highlighters.remove(reinterpret_cast<QTextDocument*>(obj));
}

void ChatSpellChecker::onSessionActivated(bool activated)
{
	qutim_sdk_0_3::ChatSession *session = static_cast<qutim_sdk_0_3::ChatSession*>(sender());
	Q_ASSERT(session);
	QObject *tmp;
	session->metaObject()->invokeMethod(m_chatForm, "textEdit", Qt::DirectConnection,
										Q_RETURN_ARG(QObject*, tmp),
										Q_ARG(qutim_sdk_0_3::ChatSession *, session));
	QWidget *textEdit = qobject_cast<QWidget*>(tmp);
	if (!qobject_cast<QPlainTextEdit*>(textEdit) &&
			!qobject_cast<QTextEdit*>(textEdit)) {
		return;
	}

	textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
	if (activated) {
		connect(textEdit, SIGNAL(customContextMenuRequested(QPoint)),
				this, SLOT(onTextEditContextMenuRequested(QPoint)),
				Qt::UniqueConnection);
	} else {
		textEdit->setContextMenuPolicy(Qt::DefaultContextMenu);
		disconnect(textEdit, SIGNAL(customContextMenuRequested(QPoint)),
				   this, SLOT(onTextEditContextMenuRequested(QPoint)));
	}
}

void ChatSpellChecker::onTextEditContextMenuRequested(const QPoint &pos)
{
	QPoint globalPos;
	QMenu *menu = 0;
	QObject *object = sender();
	if (QPlainTextEdit *textEdit = qobject_cast<QPlainTextEdit*>(object)) {
		globalPos = textEdit->mapToGlobal(pos);
#ifdef Q_WS_MAEMO_5
		menu = new QMenu();
#else
		menu = textEdit->createStandardContextMenu();
#endif
		m_cursor = textEdit->cursorForPosition(pos);
	} else if (QTextEdit *tmp = qobject_cast<QTextEdit*>(object)) {
		globalPos = tmp->mapToGlobal(pos);
#ifdef Q_WS_MAEMO_5
		menu = new QMenu();
#else
		menu = tmp->createStandardContextMenu(globalPos);
#endif
		m_cursor = tmp->cursorForPosition(pos);
	} else {
		Q_ASSERT(!"Unknown object type, check connection");
		return;
	}

	if (m_speller) {
		QTextBlock block = m_cursor.block();
		const QString blockText = block.text();
		if (!blockText.isEmpty()) {
			static QRegExp separator("\\b");
			int posInBlock = m_cursor.position() - block.position();
			m_wordBegin = blockText.lastIndexOf(separator, posInBlock);
			if (m_wordBegin != -1) {
				m_wordEnd = blockText.indexOf(separator, posInBlock);
				m_word = blockText.mid(m_wordBegin, m_wordEnd - m_wordBegin);
			}
		}

		if (!m_word.isEmpty() && !m_speller->isCorrect(m_word)) {
			QAction *before = !menu->actions().isEmpty() ? menu->actions().first() : 0;
			Q_ASSERT(m_speller);
			foreach (const QString &suggestion, m_speller->suggest(m_word).mid(0, 5))
				insertAction(menu, before, suggestion, SLOT(onSuggestionActionTriggered()));
			insertAction(menu, before, tr("Add to dictionary"), SLOT(onAddToDictionaryTriggered()));
			if (before)
				menu->insertSeparator(before);
		}
	}

	menu->popup(globalPos);
	menu->setAttribute(Qt::WA_DeleteOnClose);
}

void ChatSpellChecker::onSuggestionActionTriggered()
{
	QTextBlock block = m_cursor.block();
	m_cursor.beginEditBlock();
	m_cursor.setPosition(block.position() + m_wordBegin);
	m_cursor.setPosition(block.position() + m_wordEnd, QTextCursor::KeepAnchor);
	m_cursor.removeSelectedText();
	m_cursor.insertText(sender()->property("text").toString());
	m_cursor.endEditBlock();
}

void ChatSpellChecker::onAddToDictionaryTriggered()
{
	if (!m_speller)
		return;
	m_speller->store(m_word);
	SpellHighlighter *highlighter = m_highlighters.value(m_cursor.document());
	Q_ASSERT(highlighter);
	highlighter->rehighlightBlock(m_cursor.block());
}

void ChatSpellChecker::onDictionaryChanged()
{
	foreach (SpellHighlighter *highlighter, m_highlighters)
		highlighter->rehighlight();
}

void ChatSpellChecker::onServiceChanged(const QByteArray &name)
{
	if (name != "SpellChecker")
		return;
	connect(m_speller, SIGNAL(dictionaryChanged()), SLOT(onDictionaryChanged()));
	foreach (SpellHighlighter *highlighter, m_highlighters)
		highlighter->rehighlight();
}

void ChatSpellChecker::insertAction(QMenu *menu, QAction *before, const QString &text, const char *slot)
{
	QAction *action = new QAction(text, menu);
	QFont font = action->font();
	font.setBold(true);
	action->setFont(font);
	connect(action, SIGNAL(triggered()), slot);
	if (before)
		menu->insertAction(before, action);
	else
		menu->addAction(action);
}


} // namespace Core


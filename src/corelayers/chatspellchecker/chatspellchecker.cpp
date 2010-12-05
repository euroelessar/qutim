/****************************************************************************
 *  chatspellchecker.cpp
 *
 *  Copyright (c) 2010 by Ruslan Nigmatullin <euroelessar@gmail.com>
 *                     by Alexey Prokhin <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "chatspellchecker.h"
#include <qutim/servicemanager.h>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QContextMenuEvent>

namespace Core {

SpellHighlighter::SpellHighlighter(ChatSpellChecker *speller, QTextDocument *doc) :
		QSyntaxHighlighter(doc), m_speller(speller)
{
	m_format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
	m_format.setUnderlineColor(Qt::red);
}

void SpellHighlighter::highlightBlock(const QString &text)
{
	static QRegExp expression(QLatin1String("\\b\\w+\\b"));

	int index = text.indexOf(expression);
	while (index >= 0) {
		int length = expression.matchedLength();
		if (!m_speller->isCorrect(expression.cap(0)))
			setFormat(index, length, m_format);
		index = text.indexOf(expression, index + length);
	}
}

ChatSpellChecker::ChatSpellChecker()
{
	m_speller = SpellChecker::instance();
	if (!m_speller) {
		deleteLater();
		return;
	}
	m_chatForm = ServiceManager::getByName("ChatForm");
	connect(ChatLayer::instance(), SIGNAL(sessionCreated(qutim_sdk_0_3::ChatSession*)),
			this, SLOT(onSessionCreated(qutim_sdk_0_3::ChatSession*)));
}

bool ChatSpellChecker::isCorrect(const QString &word)
{
	Q_ASSERT(m_speller);
	return m_speller->isCorrect(word);
}

void ChatSpellChecker::onSessionCreated(qutim_sdk_0_3::ChatSession *session)
{
	QTextDocument *inputField = session->getInputField();
	SpellHighlighter *highlighter = new SpellHighlighter(this, inputField);
	if (m_chatForm) {
		connect(session, SIGNAL(activated(bool)), SLOT(onSessionActivated(bool)));
		// We use m_highlighter only in onAddToDictionaryTriggered
		// which run only when ChatForm service is enabled (i.e., m_chatForm != 0).
		m_highlighters.insert(inputField, highlighter);
		connect(inputField, SIGNAL(destroyed(QObject*)), SLOT(onInputFieldDestroyed(QObject*)));
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
	if (!textEdit &&
		!qobject_cast<QPlainTextEdit*>(textEdit) &&
		!qobject_cast<QTextEdit*>(textEdit))
	{
		return;
	}

	textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
	if (activated)
		connect(textEdit, SIGNAL(customContextMenuRequested(QPoint)),
				this, SLOT(onTextEditContextMenuRequested(QPoint)));
	else
		disconnect(textEdit, SIGNAL(customContextMenuRequested(QPoint)),
				   this, SLOT(onTextEditContextMenuRequested(QPoint)));
}

void ChatSpellChecker::onTextEditContextMenuRequested(const QPoint &pos)
{
	QPoint globalPos;
	QMenu *menu = 0;
	if (QPlainTextEdit *textEdit = qobject_cast<QPlainTextEdit*>(sender())) {
		globalPos = textEdit->mapToGlobal(pos);
		menu = textEdit->createStandardContextMenu();
		m_cursor = textEdit->cursorForPosition(pos);
	} else if (QTextEdit *tmp = qobject_cast<QTextEdit*>(sender())) {
		globalPos = textEdit->mapToGlobal(pos);
		menu = tmp->createStandardContextMenu(globalPos);
		m_cursor = textEdit->cursorForPosition(pos);
	} else {
		Q_ASSERT(0);
	}

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

	if (!m_word.isEmpty() && !isCorrect(m_word)) {
		QAction *before = !menu->actions().isEmpty() ? menu->actions().first() : 0;
		Q_ASSERT(m_speller);
		foreach (const QString &suggestion, m_speller->suggest(m_word).mid(0, 5))
			insertAction(menu, before, suggestion, SLOT(onSuggestionActionTriggered()));
		insertAction(menu, before, tr("Add to dictionary"), SLOT(onAddToDictionaryTriggered()));
		if (before)
			menu->insertSeparator(before);
	}

	menu->exec(globalPos);
	delete menu;
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
	Q_ASSERT(m_speller);
	m_speller->store(m_word);
	SpellHighlighter *highlighter = m_highlighters.value(m_cursor.document());
	Q_ASSERT(highlighter);
	highlighter->rehighlightBlock(m_cursor.block());
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

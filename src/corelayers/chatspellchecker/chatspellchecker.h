/****************************************************************************
 *  chatspellchecker.h
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

#ifndef CHATSPELLCHECKER_H
#define CHATSPELLCHECKER_H

#include <qutim/startupmodule.h>
#include <qutim/settingslayer.h>
#include <qutim/messagesession.h>
#include <qutim/spellchecker.h>
#include <QSyntaxHighlighter>
#include <QTextCursor>
#include <QMetaMethod>

namespace Core {

using namespace qutim_sdk_0_3;

class ChatSpellChecker;

class SpellHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	explicit SpellHighlighter(ChatSpellChecker *checker, QTextDocument *doc);
	virtual void highlightBlock(const QString &text);
private:
	ChatSpellChecker *m_speller;
	QTextCharFormat m_format;
};

class ChatSpellChecker : public QObject, public StartupModule
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::StartupModule)
public:
	ChatSpellChecker();
	bool isCorrect(const QString &word);
private slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession*);
	void onInputFieldDestroyed(QObject *obj);
	void onSessionActivated(bool activated);
	void onTextEditContextMenuRequested(const QPoint &pos);
	void onSuggestionActionTriggered();
	void onAddToDictionaryTriggered();
	void onDictionaryChanged();
private:
	void insertAction(QMenu *menu, QAction *before, const QString &text, const char *slot);
private:
	SpellChecker *m_speller;
	QObject *m_chatForm;
	QTextCursor m_cursor;
	QString m_word;
	int m_wordBegin, m_wordEnd;
	QHash<QTextDocument*,SpellHighlighter*> m_highlighters;
};

} // namespace Core

#endif // CHATSPELLCHECKER_H

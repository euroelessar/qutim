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
#include <qutim/servicemanager.h>
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
	explicit SpellHighlighter(QTextDocument *doc);
	virtual void highlightBlock(const QString &text);
private:
	qutim_sdk_0_3::ServicePointer<SpellChecker> m_speller;
	QTextCharFormat m_format;
};

class ChatSpellChecker : public QObject, public StartupModule
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::StartupModule)
public:
	ChatSpellChecker();
private slots:
	void onSessionCreated(qutim_sdk_0_3::ChatSession*);
	void onInputFieldDestroyed(QObject *obj);
	void onSessionActivated(bool activated);
	void onTextEditContextMenuRequested(const QPoint &pos);
	void onSuggestionActionTriggered();
	void onAddToDictionaryTriggered();
	void onDictionaryChanged();
	void onServiceChanged(const QByteArray &name);
private:
	void insertAction(QMenu *menu, QAction *before, const QString &text, const char *slot);
private:
	ServicePointer<QObject> m_chatForm;
	ServicePointer<SpellChecker> m_speller;
	QTextCursor m_cursor;
	QString m_word;
	int m_wordBegin, m_wordEnd;
	QHash<QTextDocument*,SpellHighlighter*> m_highlighters;
};

} // namespace Core

#endif // CHATSPELLCHECKER_H

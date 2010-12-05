/****************************************************************************
 *  hunspellchecker.h
 *
 *  Copyright (c) 2010 by Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef HUNSPELLCHECKER_H
#define HUNSPELLCHECKER_H

#include <qutim/messagesession.h>
#include <qutim/spellchecker.h>
#include "hunspell/hunspell.hxx"
#include <QObject>

using namespace qutim_sdk_0_3;

class HunSpellChecker : public SpellChecker
{
	Q_OBJECT
public:
	explicit HunSpellChecker();
	virtual ~HunSpellChecker();
	QStringList languages() const;
	bool isCorrect(const QString &word) const;
	QStringList suggest(const QString &word) const;
	void store(const QString &word) const;
	void storeReplacement(const QString &bad, const QString &good);
	static HunSpellChecker *instance() { Q_ASSERT(self); return self; }
	static QString toPrettyLanguageName(QString lang);
	void loadSettings(QString lang);
	QByteArray convert(const QString &word) const;
private:
	Hunspell *m_speller;
	QString m_dictPath;
	QTextCodec *m_codec;
	static HunSpellChecker *self;
};

#endif // ASPELLCHECKER_H

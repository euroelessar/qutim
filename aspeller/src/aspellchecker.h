/****************************************************************************
 * aspellchecker.h
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

#ifndef ASPELLCHECKER_H
#define ASPELLCHECKER_H

#include <qutim/messagesession.h>
#include <qutim/spellchecker.h>

#include <QObject>

struct AspellConfig;
struct AspellSpeller;

using namespace qutim_sdk_0_3;

class ASpellChecker : public SpellChecker
{
	Q_OBJECT
public:
	explicit ASpellChecker();
	virtual ~ASpellChecker();
	QStringList languages() const;
	bool isCorrect(const QString &word) const;
	bool isMisspelled(const QString &word) const;
	QStringList suggest(const QString &word) const;
	void store(const QString &word) const;
	void storeReplacement(const QString &bad, const QString &good);
	static ASpellChecker *instance() { Q_ASSERT(self); return self; }
	static QString toPrettyLanguageName(const QString &lang);
	void loadSettings(const QString &lang);
private:
	AspellConfig  *m_config;
	AspellSpeller *m_speller;
	static ASpellChecker *self;
};

#endif // ASPELLCHECKER_H

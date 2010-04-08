/****************************************************************************
 * spellhighlighter.cpp
 *
 *  Copyright (c) 2010 by Ruslan Nigmatullin <euroelessar@gmail.com>
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

#include "spellhighlighter.h"
#include "aspellchecker.h"

namespace ASpell
{
SpellHighlighter::SpellHighlighter(ASpellChecker *speller, QTextDocument *doc) :
		QSyntaxHighlighter(doc), m_speller(speller)
{
}

void SpellHighlighter::highlightBlock(const QString &text)
{
	QTextCharFormat format;
	format.setUnderlineStyle(QTextCharFormat::SpellCheckUnderline);
	format.setUnderlineColor(Qt::red);

	QRegExp expression(QLatin1String("\\b\\w+\\b"));

	int index = text.indexOf(expression);
	while (index >= 0) {
		int length = expression.matchedLength();
		if (!m_speller->isCorrect(expression.cap(0)))
			setFormat(index, length, format);
		index = text.indexOf(expression, index + length);
	}
}
}

/****************************************************************************
 * spellhighlighter.h
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

#ifndef SPELLHIGHLIGHTER_H
#define SPELLHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <qutim/messagesession.h>

class ASpellChecker;
using namespace qutim_sdk_0_3;

namespace ASpell
{
class SpellHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	explicit SpellHighlighter(ASpellChecker *checker, QTextDocument *doc);
	virtual void highlightBlock(const QString &text);
private:
	ASpellChecker *m_speller;
};
}

#endif // SPELLHIGHLIGHTER_H

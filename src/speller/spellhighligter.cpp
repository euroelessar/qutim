#include "spellhighligter.h"

namespace Kde
{
SpellHighlighter::SpellHighlighter(Sonnet::Speller *speller, QTextDocument *doc) :
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

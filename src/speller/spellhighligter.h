#ifndef SPELLHIGHLIGTER_H
#define SPELLHIGHLIGTER_H

#include <QSyntaxHighlighter>
#include <sonnet/speller.h>

namespace Kde
{
class SpellHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	explicit SpellHighlighter(Sonnet::Speller *speller, QTextDocument *doc);
	virtual void highlightBlock(const QString &text);
private:
	Sonnet::Speller *m_speller;
};
}

#endif // SPELLHIGHLIGTER_H

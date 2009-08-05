#include "spellerhighlighter.h"

SpellerHighlighter::SpellerHighlighter( QTextEdit *edit ) : QSyntaxHighlighter(edit)
{
	m_active = true;
}

void SpellerHighlighter::setActive( bool active )
{
	if( m_active == active )
		return;
	m_active = active;
	rehighlight();
}

void SpellerHighlighter::highlightBlock ( const QString &text )
{
#if 0
	static QRegExp regexp("\\b\\w+\\b");
	int pos = 0;
	while( ( ( pos = regexp.indexIn( text, pos ) ) != -1 ) )
	{
		int length = regexp.matchedLength();
		if( !MacSpeller::instance().isCorrect( regexp.cap() ) )
			setFormat( pos, length, QTextCharFormat::SpellCheckUnderline );
		pos += length;
	}
#else
	Q_UNUSED(text);
#endif
}

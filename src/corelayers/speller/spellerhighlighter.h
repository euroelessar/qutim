#ifndef SPELLERHIGHLIGHTER_H
#define SPELLERHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextEdit>

class SpellerHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT
public:
	SpellerHighlighter( QTextEdit *edit );
	void setActive( bool active );
protected:
	virtual void highlightBlock( const QString &text );
private:
	bool m_active;
};

#endif // SPELLERHIGHLIGHTER_H

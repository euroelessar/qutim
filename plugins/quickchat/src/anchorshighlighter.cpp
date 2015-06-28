#include "anchorshighlighter.h"
#include <QAbstractTextDocumentLayout>
#include <QMetaMethod>
#include <QTimer>
#include <QPalette>
#include <QGuiApplication>

namespace QuickChat {

AnchorsHighlighter::AnchorsHighlighter() :
	QSyntaxHighlighter(static_cast<QTextDocument *>(nullptr))
{
	QPalette palette = QGuiApplication::palette();
	m_activeColor = palette.link().color();
	m_activeStyle = Qt::NoPen;
	m_hoverColor = palette.link().color();
	m_hoverStyle = Qt::SolidLine;
}

void AnchorsHighlighter::highlightBlock(const QString &text)
{
	Q_UNUSED(text);
	const QTextBlock block = currentBlock();
	const int position = m_position - block.position();

	QTextCharFormat hoverFormat;
	hoverFormat.setForeground(m_hoverColor);
	hoverFormat.setUnderlineStyle(static_cast<QTextCharFormat::UnderlineStyle>(m_hoverStyle));

	QTextCharFormat activeFormat;
	activeFormat.setForeground(m_activeColor);
	activeFormat.setUnderlineStyle(static_cast<QTextCharFormat::UnderlineStyle>(m_activeStyle));

	foreach (const QTextLayout::FormatRange &range, block.textFormats()) {
		const QString anchorHref = range.format.anchorHref();
		if (anchorHref.isEmpty())
			continue;

		// This is a special case, do not highlight
		if (anchorHref.startsWith(QStringLiteral("session:"))
				&& anchorHref.endsWith(QStringLiteral("#do-not-style"))) {
			continue;
		}

		const bool hover = position >= range.start && position < range.start + range.length;
		setFormat(range.start, range.length, hover ? hoverFormat : activeFormat);
	}
}

QQuickTextDocument *AnchorsHighlighter::textDocument() const
{
	return m_textDocument;
}

void AnchorsHighlighter::setTextDocument(QQuickTextDocument *textDocument)
{
	if (m_textDocument == textDocument)
		return;

	m_textDocument = textDocument;
	if (m_textDocument)
		setDocument(m_textDocument->textDocument());
	else
		setDocument(nullptr);
	emit textDocumentChanged(textDocument);
}

int AnchorsHighlighter::potision() const
{
	return m_position;
}

void AnchorsHighlighter::setPosition(int position)
{
	if (m_position == position)
		return;

	QTextBlock oldBlock = blockAt(m_position);
	QTextBlock block = blockAt(position);

	m_position = position;
	emit positionChanged(position);

	if (oldBlock.isValid())
		rehighlightBlock(oldBlock);

	if (block.isValid() && oldBlock != block)
		rehighlightBlock(block);
}

QColor AnchorsHighlighter::activeColor() const
{
	return m_activeColor;
}

void AnchorsHighlighter::setActiveColor(QColor activeColor)
{
	if (m_activeColor == activeColor)
		return;

	m_activeColor = activeColor;
	emit activeColorChanged(activeColor);

	rehighlight();
}

Qt::PenStyle AnchorsHighlighter::activeStyle() const
{
	return m_activeStyle;
}

void AnchorsHighlighter::setActiveStyle(Qt::PenStyle activeStyle)
{
	if (m_activeStyle == activeStyle)
		return;

	m_activeStyle = activeStyle;
	emit activeStyleChanged(activeStyle);

	rehighlight();
}

QColor AnchorsHighlighter::hoverColor() const
{
	return m_hoverColor;
}

void AnchorsHighlighter::setHoverColor(QColor hoverColor)
{
	if (m_hoverColor == hoverColor)
		return;

	m_hoverColor = hoverColor;
	emit hoverColorChanged(hoverColor);

	rehighlight();
}

Qt::PenStyle AnchorsHighlighter::hoverStyle() const
{
	return m_hoverStyle;
}

void AnchorsHighlighter::setHoverStyle(Qt::PenStyle hoverStyle)
{
	if (m_hoverStyle == hoverStyle)
		return;

	m_hoverStyle = hoverStyle;
	emit hoverStyleChanged(hoverStyle);

	rehighlight();
}

QTextBlock AnchorsHighlighter::blockAt(int position)
{
	if (position < 0)
		return QTextBlock();
	QTextCursor cursor(document());
	cursor.setPosition(position);
	return cursor.block();
}

} // namespace QuickChat

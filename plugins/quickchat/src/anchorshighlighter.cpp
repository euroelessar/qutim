#include "anchorshighlighter.h"
#include <QAbstractTextDocumentLayout>
#include <QMetaMethod>
#include <QTimer>

namespace QuickChat {

AnchorsHighlighter::AnchorsHighlighter() :
    QSyntaxHighlighter(static_cast<QTextDocument *>(nullptr))
{
}

void AnchorsHighlighter::highlightBlock(const QString &text)
{
    Q_UNUSED(text);
    const QTextBlock block = currentBlock();
    const int position = m_position - block.position();

    foreach (const QTextLayout::FormatRange &range, block.textFormats()) {
        const QString anchorHref = range.format.anchorHref();
        if (anchorHref.isEmpty())
            continue;

        QTextCharFormat format;
        if (position >= range.start && position < range.start + range.length) {
            format.setForeground(QColor(Qt::red));
            format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
        } else {
            format.setForeground(QColor(Qt::blue));
            format.setUnderlineStyle(QTextCharFormat::NoUnderline);
        }
        setFormat(range.start, range.length, format);
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

QTextBlock AnchorsHighlighter::blockAt(int position)
{
    if (position < 0)
        return QTextBlock();
    QTextCursor cursor(document());
    cursor.setPosition(position);
    return cursor.block();
}

} // namespace QuickChat

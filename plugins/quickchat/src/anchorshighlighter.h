#ifndef QUICKCHAT_ANCHORSHIGHLIGHTER_H
#define QUICKCHAT_ANCHORSHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QQuickTextDocument>
#include <qutim/utils.h>

namespace QuickChat {

class AnchorsHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
    Q_PROPERTY(QQuickTextDocument* textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)
    Q_PROPERTY(int position READ potision WRITE setPosition NOTIFY positionChanged)
public:
    explicit AnchorsHighlighter();

    void highlightBlock(const QString &text) override;

    QQuickTextDocument *textDocument() const;
    void setTextDocument(QQuickTextDocument *textDocument);

    int potision() const;
    void setPosition(int position);

signals:
    void textDocumentChanged(QQuickTextDocument *textDocument);
    void positionChanged(int position);

private:
    QTextBlock blockAt(int position);

    QQuickTextDocument *m_textDocument = nullptr;
    int m_position = -1;
};

} // namespace QuickChat

#endif // QUICKCHAT_ANCHORSHIGHLIGHTER_H

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
	Q_PROPERTY(QColor activeColor READ activeColor WRITE setActiveColor NOTIFY activeColorChanged)
	Q_PROPERTY(Qt::PenStyle activeStyle READ activeStyle WRITE setActiveStyle NOTIFY activeStyleChanged)
	Q_PROPERTY(QColor hoverColor READ hoverColor WRITE setHoverColor NOTIFY hoverColorChanged)
	Q_PROPERTY(Qt::PenStyle hoverStyle READ hoverStyle WRITE setHoverStyle NOTIFY hoverStyleChanged)
public:
	explicit AnchorsHighlighter();

	void highlightBlock(const QString &text) override;

	QQuickTextDocument *textDocument() const;
	void setTextDocument(QQuickTextDocument *textDocument);

	int potision() const;
	void setPosition(int position);

	QColor activeColor() const;
	void setActiveColor(QColor activeColor);

	Qt::PenStyle activeStyle() const;
	void setActiveStyle(Qt::PenStyle activeStyle);

	QColor hoverColor() const;
	void setHoverColor(QColor hoverColor);

	Qt::PenStyle hoverStyle() const;
	void setHoverStyle(Qt::PenStyle hoverStyle);

signals:
	void textDocumentChanged(QQuickTextDocument *textDocument);
	void positionChanged(int position);
	void activeColorChanged(QColor activeColor);
	void activeStyleChanged(Qt::PenStyle activeStyle);
	void hoverColorChanged(QColor hoverColor);
	void hoverStyleChanged(Qt::PenStyle hoverStyle);

private:
	QTextBlock blockAt(int position);

	QQuickTextDocument *m_textDocument = nullptr;
	int m_position = -1;
	QColor m_activeColor;
	Qt::PenStyle m_activeStyle;
	QColor m_hoverColor;
	Qt::PenStyle m_hoverStyle;
};

} // namespace QuickChat

#endif // QUICKCHAT_ANCHORSHIGHLIGHTER_H

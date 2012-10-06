#ifndef MACSEARCHFIELD_H
#define MACSEARCHFIELD_H

#include <QMacCocoaViewContainer>

class MacSearchFieldWidget;

class MacSearchField : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
public:
	MacSearchField(QWidget *parent = 0);
	~MacSearchField();

	QString text() const;
	void setText(const QString &text);

	QSize sizeHint() const;

signals:
	void textChanged(const QString &text);

protected:
	void resizeEvent(QResizeEvent *);

private:
	friend class MacSearchFieldWidget;
	MacSearchFieldWidget *m_widget;
	QString m_text;
};

#endif // MACSEARCHFIELD_H

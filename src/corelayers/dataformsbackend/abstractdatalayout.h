#ifndef ABSTRACTDATALAYOUT_H
#define ABSTRACTDATALAYOUT_H

#include <QWidget>
#include <QGridLayout>
#include <QHostAddress>
#include <libqutim/dataforms.h>

class QLabel;
class QPushButton;

Q_DECLARE_METATYPE(QHostAddress);

namespace Core
{
using namespace qutim_sdk_0_3;

class AbstractDataLayout : public QGridLayout
{
public:
	AbstractDataLayout(QWidget *parent = 0);
	~AbstractDataLayout();
	virtual bool addItem(const DataItem &item) = 0;
	bool addItems(const QList<DataItem> &items);
	void addSpacer();
	void addWidget(QWidget *w);
protected:
	void addWidget(QWidget *, int row, int column, Qt::Alignment = 0);
	void addWidget(QWidget *, int row, int column, int rowSpan, int columnSpan, Qt::Alignment = 0);
	void addLayout(QLayout *, int row, int column, Qt::Alignment = 0);
	void addLayout(QLayout *, int row, int column, int rowSpan, int columnSpan, Qt::Alignment = 0);
	int m_row;
};

inline void AbstractDataLayout::addWidget(QWidget *w, int row, int column, Qt::Alignment alignment)
{
	QGridLayout::addWidget(w, row, column, alignment);
}

inline void AbstractDataLayout::addWidget(QWidget *w, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment)
{
	QGridLayout::addWidget(w, row, column, rowSpan, columnSpan, alignment);
}

inline void AbstractDataLayout::addLayout(QLayout *w, int row, int column, Qt::Alignment alignment)
{
	QGridLayout::addLayout(w, row, column, alignment);
}

inline void AbstractDataLayout::addLayout(QLayout *w, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment)
{
	QGridLayout::addLayout(w, row, column, rowSpan, columnSpan, alignment);
}

QPixmap variantToPixmap(const QVariant &data, const QSize &size);

}

#endif // ABSTRACTDATALAYOUT_H

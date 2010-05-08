#ifndef INFOLAYOUT_H
#define INFOLAYOUT_H

#include <QWidget>
#include <QGridLayout>
#include <QHostAddress>
#include <libqutim/inforequest.h>

class QLabel;
class QPushButton;

Q_DECLARE_METATYPE(QHostAddress);

namespace Core
{
using namespace qutim_sdk_0_3;

class AbstractInfoLayout : public QGridLayout
{
public:
	AbstractInfoLayout(QWidget *parent = 0);
	~AbstractInfoLayout();
	void addSpacer();
	virtual bool addItem(const InfoItem &item) = 0;
	virtual bool addItems(const QList<InfoItem> &items) = 0;
protected:
	void addWidget(QWidget *w);
	void addWidget(QWidget *, int row, int column, Qt::Alignment = 0);
	void addWidget(QWidget *, int row, int column, int rowSpan, int columnSpan, Qt::Alignment = 0);
	void addLayout(QLayout *, int row, int column, Qt::Alignment = 0);
	void addLayout(QLayout *, int row, int column, int rowSpan, int columnSpan, Qt::Alignment = 0);
	int m_row;
};

inline void AbstractInfoLayout::addWidget(QWidget *w)
{
	QGridLayout::addWidget(w);
}

inline void AbstractInfoLayout::addWidget(QWidget *w, int row, int column, Qt::Alignment alignment)
{
	QGridLayout::addWidget(w, row, column, alignment);
}

inline void AbstractInfoLayout::addWidget(QWidget *w, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment)
{
	QGridLayout::addWidget(w, row, column, rowSpan, columnSpan, alignment);
}

inline void AbstractInfoLayout::addLayout(QLayout *w, int row, int column, Qt::Alignment alignment)
{
	QGridLayout::addLayout(w, row, column, alignment);
}

inline void AbstractInfoLayout::addLayout(QLayout *w, int row, int column, int rowSpan, int columnSpan, Qt::Alignment alignment)
{
	QGridLayout::addLayout(w, row, column, rowSpan, columnSpan, alignment);
}

}

#endif // INFOLAYOUT_H

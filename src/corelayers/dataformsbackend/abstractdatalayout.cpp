#include "abstractdatalayout.h"
#include <QSpacerItem>

namespace Core
{

AbstractDataLayout::AbstractDataLayout(QWidget *parent) :
	QGridLayout(parent), m_row(0)
{
}

AbstractDataLayout::~AbstractDataLayout()
{
}

bool AbstractDataLayout::addItems(const QList<DataItem> &items)
{
	bool expand = false;
	foreach (const DataItem &item, items)
		expand = addItem(item) || expand;
	return expand;
}

void AbstractDataLayout::addSpacer()
{
	QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	QGridLayout::addItem(spacer, m_row++, 0);
}

void  AbstractDataLayout::addWidget(QWidget *w)
{
	QGridLayout::addWidget(w, m_row++, 0, 1, 2);
}

}

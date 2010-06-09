#include "abstractdatalayout.h"
#include <QSpacerItem>
#include <QIcon>

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

QPixmap variantToPixmap(const QVariant &data, const QSize &size)
{
	int type = data.type();
	QPixmap pixmap;
	if (type == QVariant::Icon)
		return data.value<QIcon>().pixmap(size); // The pixmap can be returned as its size is correct.
	else if (type == QVariant::Pixmap)
		pixmap = data.value<QPixmap>();
	else if (type == QVariant::Image)
		pixmap = QPixmap::fromImage(data.value<QImage>());
	if (!pixmap.isNull())
		return pixmap.scaled(size, Qt::KeepAspectRatio);
	else
		return pixmap;
}

}

#include "abstractdatalayout.h"
#include <QSpacerItem>
#include <QIcon>
#include <QStyle>
#include <QApplication>

namespace Core
{

AbstractDataLayout::AbstractDataLayout(QWidget *parent) :
	QGridLayout(parent), m_style(0), m_row(0)
{
	m_labelAlignment = Qt::Alignment(getStyle()->styleHint(QStyle::SH_FormLayoutLabelAlignment));
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

void AbstractDataLayout::addRow(QWidget *title, QWidget *widget, Qt::Alignment widgetAligment)
{
	if (title) {
		addWidget(title, m_row, 0, m_labelAlignment);
#ifdef QUTIM_MOBILE_UI
		++m_row;
		addWidget(widget, m_row++, 0, widgetAligment);
#else
		addWidget(widget, m_row++, 1, widgetAligment);
#endif
	} else {
#ifdef QUTIM_MOBILE_UI
		addWidget(widget, m_row++, 0);
#else
		addWidget(widget, m_row++, 0, 1, 2);
#endif
	}
}

QStyle* AbstractDataLayout::getStyle() const
{
	if (!m_style) {
		if (QWidget *parent = parentWidget())
			m_style = parent->style();
		else
			m_style = QApplication::style();
	}
	return m_style.data();
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

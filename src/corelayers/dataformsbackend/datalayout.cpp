#include "datalayout.h"
#include "widgetgenerator.h"
#include <QSpacerItem>
#include <QIcon>
#include <QStyle>
#include <QApplication>

namespace Core
{

DataLayout::DataLayout(QWidget *parent) :
	QGridLayout(parent), m_style(0), m_row(0), m_expandable(false)
{
}

DataLayout::~DataLayout()
{
}

DataItem DataLayout::item() const
{
	DataItem items;
	items.setName(objectName());
	foreach (const WidgetLine &line, m_widgets)
		items.addSubitem(getDataItem(line.title, line.data));
	return items;
}

void DataLayout::addItem(const DataItem &item)
{
	bool twoColumns;
	QWidget *widget = getWidget(item, parentWidget(), &twoColumns);
	QWidget *title = 0;
	twoColumns = twoColumns || item.property("hideTitle", false);
	if (!twoColumns)
		title = getTitle(item, labelAlignment(), parentWidget());
	widget->setObjectName(item.name());
	addRow(title, widget, !item.isReadOnly() ?  Qt::Alignment() : Qt::AlignLeft);
	m_widgets.push_back(WidgetLine(title, widget));
	if (!item.isReadOnly()) {
		QSizePolicy::Policy policy = widget->sizePolicy().verticalPolicy();
		if (!m_expandable)
			m_expandable = policy == QSizePolicy::MinimumExpanding || policy == QSizePolicy::Expanding;
	}
}

void DataLayout::addItems(const QList<DataItem> &items)
{
	foreach (const DataItem &item, items)
		addItem(item);
}

void DataLayout::addSpacer()
{
	QSpacerItem *spacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);
	QGridLayout::addItem(spacer, m_row++, 0);
}

void DataLayout::addRow(QWidget *title, QWidget *widget, Qt::Alignment widgetAligment)
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

Qt::Alignment DataLayout::labelAlignment()
{
	if (!m_style) {
		if (QWidget *parent = parentWidget())
			m_style = parent->style();
		else
			m_style = QApplication::style();
		m_labelAlignment = Qt::Alignment(m_style->styleHint(QStyle::SH_FormLayoutLabelAlignment));
	}
	return m_labelAlignment;
}

}

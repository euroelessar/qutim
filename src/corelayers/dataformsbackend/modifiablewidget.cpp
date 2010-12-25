#include "modifiablewidget.h"
#include "widgetgenerator.h"
#include <QGridLayout>
#include <QPushButton>
#include <QStyle>
#include <QApplication>
#include <qutim/icon.h>

namespace Core {

ModifiableWidget::ModifiableWidget(DefaultDataForm *dataForm, QWidget *parent) :
		QWidget(parent),
		AbstractDataWidget(dataForm)
{
	init();
}

ModifiableWidget::ModifiableWidget(const DataItem &item, DefaultDataForm *dataForm, QWidget *parent) :
		QWidget(parent),
		AbstractDataWidget(dataForm),
		m_def(item.defaultSubitem()),
		m_max(item.maxSubitemsCount())
{
	init();
	foreach (const DataItem &subitem, item.subitems())
		addRow(subitem);
}

ModifiableWidget::~ModifiableWidget()
{
}

void ModifiableWidget::init()
{
	m_max = -1;
	m_layout = new QGridLayout(this);
	m_addButton = new QPushButton(this);
	m_addButton->setIcon(Icon("list-add"));
	connect(m_addButton, SIGNAL(clicked()), SLOT(onAddRow()));
	m_layout->addWidget(m_addButton, 0, 1, 1, 1);
}

Qt::Alignment ModifiableWidget::labelAlignment() const
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

void ModifiableWidget::addRow(QWidget *data, QWidget *title)
{
	data->setParent(this);
	if (title)
		title->setParent(this);
	QPushButton *deleteButton = new QPushButton(this);
	deleteButton->setIcon(Icon("list-remove"));
	connect(deleteButton, SIGNAL(clicked()), SLOT(onRemoveRow()));
	deleteButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	int row = m_widgets.size();
	WidgetLine line(deleteButton, data, title);
	m_widgets.push_back(line);
	m_layout->removeWidget(m_addButton);
	setRow(line, row);
#ifdef QUTIM_MOBILE_UI
	m_layout->addWidget(m_addButton, row*2 + 2, 1);
#else
	m_layout->addWidget(m_addButton, ++row, 2, 1, 1);
#endif
	m_addButton->setVisible(m_max < 0 || m_max > m_widgets.count());
	emit rowAdded();
}

void ModifiableWidget::addRow(const DataItem &item)
{
	QWidget *title = 0;
	bool twoColumn;
	QWidget *data = getWidget(dataForm(), item, this, &twoColumn);
	data->setObjectName(item.name());
	if (!twoColumn && !item.property("hideTitle", false))
		title = getTitle(dataForm(), item, labelAlignment(), this);
	addRow(data, title);
}

DataItem ModifiableWidget::item() const
{
	DataItem items;
	items.setName(objectName());
	items.allowModifySubitems(m_def, m_max);
	foreach (const WidgetLine &line, m_widgets)
		items.addSubitem(getDataItem(line.title, line.data));
	return items;
}

bool ModifiableWidget::isExpandable()
{
	QVariant data = m_def.data();
	QVariant alt = m_def.property("alternatives");
	bool isLineEdit = (data.canConvert(QVariant::String) && alt.canConvert(QVariant::StringList)) ||
					  (qVariantCanConvert<LocalizedString>(data) && qVariantCanConvert<LocalizedStringList>(alt));
	return isLineEdit && m_def.property("multiline", false);
}

void ModifiableWidget::clear()
{
	foreach (const WidgetLine &line, m_widgets) {
		line.deleteButton->deleteLater();
		line.data->deleteLater();
		if (line.title)
			line.title->deleteLater();
	}
	m_widgets.clear();
	m_addButton->setVisible(false);
}

void ModifiableWidget::onAddRow()
{
	addRow(m_def);
}

void ModifiableWidget::onRemoveRow()
{
	QWidget *deleteButton = reinterpret_cast<QWidget*>(sender());
	Q_ASSERT(deleteButton);
	WidgetList::iterator itr = m_widgets.begin();
	int row = 0;
	while (itr->deleteButton != deleteButton) {
		Q_ASSERT(itr != m_widgets.end());
		++itr;
		++row;
	}
	deleteButton->deleteLater();
	itr->data->deleteLater();
	if (itr->title)
		itr->title->deleteLater();
	itr = m_widgets.erase(itr);
	while (itr != m_widgets.end()) {
		m_layout->removeWidget(itr->deleteButton);
		m_layout->removeWidget(itr->data);
		if (itr->title)
			m_layout->removeWidget(itr->title);
		setRow(*itr, row++);
		itr++;
	}
	if (m_max < 0 || m_max > m_widgets.count())
		m_addButton->setVisible(true);
	emit rowRemoved();
}

void ModifiableWidget::setRow(const WidgetLine &line, int row)
{
#ifdef QUTIM_MOBILE_UI
	row *= 2;
	if (line.title) {
		m_layout->addWidget(line.title, row++, 0, 1, 2, labelAlignment());
		m_layout->addWidget(line.data, row, 0);
	} else {
		m_layout->addWidget(line.data, row, 0);
	}
	m_layout->addWidget(line.deleteButton, row, 1);
#else
	if (line.title) {
		m_layout->addWidget(line.title, row, 0, labelAlignment());
		m_layout->addWidget(line.data, row, 1);
	} else {
		m_layout->addWidget(line.data, row, 0, 1, 2);
	}
	m_layout->addWidget(line.deleteButton, row, 2);
#endif
}

} // namespace Core

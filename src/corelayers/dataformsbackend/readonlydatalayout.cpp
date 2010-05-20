#include "readonlydatalayout.h"
#include <QGroupBox>
#include <QLabel>
#include <QDate>
#include <QDateEdit>

namespace Core
{

ReadOnlyDataLayout::ReadOnlyDataLayout(QWidget *parent) :
	AbstractDataLayout(parent)
{
}

bool ReadOnlyDataLayout::addItem(const DataItem &item)
{
	bool twoColumns;
	QWidget *widget = getReadOnlyWidget(item, &twoColumns);
	if (!twoColumns && !item.property("hideTitle", false)) {
		QLabel *title = new QLabel(item.title().toString() + ":", parentWidget());
		title->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
		QFont font = title->font();
		font.setBold(true);
		title->setFont(font);
		title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
		addWidget(title, m_row, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);
	}
	widget->setParent(parentWidget());
	widget->setObjectName(item.name());
	if (!twoColumns)
		addWidget(widget, m_row++, 1, 1, 1, Qt::AlignLeft);
	else
		addWidget(widget, m_row++, 0, 1, 2);
	return false;
}

QWidget *ReadOnlyDataLayout::getReadOnlyWidget(const DataItem &item, bool *twoColumn)
{
	if (twoColumn)
		*twoColumn = false;
	if (item.hasSubitems()) {
		QGroupBox *box = new QGroupBox();
		box->setTitle(item.title());
		box->setFlat(true);
		ReadOnlyDataLayout *group = new ReadOnlyDataLayout(box);
		group->addItems(item.subitems());
		if (twoColumn)
			*twoColumn = true;
		return box;
	}
	bool enabled = true;
	QVariant::Type type = item.data().type();
	if (item.property("notSet", false)) {
		enabled = false;
	} else if (type == QVariant::Date) {
		return getLabel(item.data().toDate().toString(Qt::SystemLocaleLongDate));
	} else if (type == QVariant::DateTime) {
		return getLabel(item.data().toDateTime().toString(Qt::SystemLocaleLongDate));
	} else if (type == QVariant::Image) {
		QLabel *d = new QLabel();
		d->setPixmap(QPixmap::fromImage(item.data().value<QImage>()));
		return d;
	} else if (type == QVariant::Pixmap) {
		QLabel *d = new QLabel();
		d->setPixmap(item.data().value<QPixmap>());
		return d;
	} else if (type == QVariant::Bool) {
		return getLabel(item.data().toBool() ?
						QT_TRANSLATE_NOOP("DataForms", "yes") :
						QT_TRANSLATE_NOOP("DataForms", "no"));
	} else if (item.data().canConvert<QHostAddress>()) {
		QHostAddress address = item.data().value<QHostAddress>();
		if (!address.isNull())
			return getLabel(address.toString());
	} else if (type == QVariant::StringList) {
		return getLabel(item.data().toStringList().join("<br>"));
	}
	QString str;
	if (enabled) {
		str = item.data().toString();
		enabled = !str.isEmpty();
	}
	if (!enabled)
		str = QT_TRANSLATE_NOOP("DataForms", "the field is not set");
	QLabel *label = getLabel(str);
	label->setEnabled(enabled);
	return label;
}

QLabel *ReadOnlyDataLayout::getLabel(const QString &str)
{
	QLabel *d = new QLabel(str);
	d->setTextInteractionFlags(Qt::LinksAccessibleByMouse |
							Qt::LinksAccessibleByKeyboard |
							Qt::TextSelectableByMouse |
							Qt::TextSelectableByKeyboard);
	d->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	return d;
}

}

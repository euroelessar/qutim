#include "readonlyinfolayout.h"
#include <QGroupBox>
#include <QLabel>
#include <QDate>
#include <QDateEdit>

namespace Core
{

ReadOnlyInfoLayout::ReadOnlyInfoLayout(QWidget *parent) :
	AbstractInfoLayout(parent)
{
}

bool ReadOnlyInfoLayout::addItems(const QList<InfoItem> &items)
{
	foreach (const InfoItem &item, items) {
		if (item.hasSubitems()) {
			QGroupBox *box = new QGroupBox(parentWidget());
			box->setTitle(item.title());
			box->setFlat(true);
			ReadOnlyInfoLayout *group = new ReadOnlyInfoLayout(box);
			group->addItems(item.subitems());
			addWidget(box, m_row++, 0, 1, 2);
		} else {
			addItem(item);
		}
	}
	return false;
}

bool ReadOnlyInfoLayout::addItem(const InfoItem &item)
{
	if (!item.property("hideTitle", false)) {
		QLabel *title = new QLabel(item.title().toString() + ":", parentWidget());
		title->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
		QFont font = title->font();
		font.setBold(true);
		title->setFont(font);
		title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
		addWidget(title, m_row, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);
	}
	QWidget *widget = getReadOnlyWidget(item);
	widget->setParent(parentWidget());
	widget->setObjectName(item.name());
	addWidget(widget, m_row++, 1, 1, 1, Qt::AlignLeft);
	return false;
}

QWidget *ReadOnlyInfoLayout::getReadOnlyWidget(const InfoItem &item)
{
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
						QT_TRANSLATE_NOOP("ContactInfo", "yes") :
						QT_TRANSLATE_NOOP("ContactInfo", "no"));
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
		str = QT_TRANSLATE_NOOP("ContactInfo", "the field is not set");
	QLabel *label = getLabel(str);
	label->setEnabled(enabled);
	return label;
}

QLabel *ReadOnlyInfoLayout::getLabel(const QString &str)
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

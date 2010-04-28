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

void ReadOnlyInfoLayout::addItems(const QList<InfoItem> &items)
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
}

void ReadOnlyInfoLayout::addItem(const InfoItem &item)
{
	if (!item.property("hideTitle", false)) {
		QLabel *title = new QLabel(item.title().toString() + ":", parentWidget());
		title->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
		QFont font;
		font.setBold(true);
		title->setFont(font);
		title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
		addWidget(title, m_row, 0, 1, 1, Qt::AlignRight | Qt::AlignTop);
	}
	bool enabled = true;
	QVariant::Type type = item.data().type();
	if (item.property("notSet", false)) {
		enabled = false;
	} else if (type == QVariant::Date) {
		addLabel(item.data().toDate().toString(Qt::SystemLocaleLongDate), item.name());
		return;
	} else if (type == QVariant::DateTime) {
		addLabel(item.data().toDateTime().toString(Qt::SystemLocaleLongDate), item.name());
		return;
	} else if (type == QVariant::Image) {
		QLabel *d = new QLabel();
		d->setPixmap(QPixmap::fromImage(item.data().value<QImage>()));
		addDataWidget(d, item.name());
		return;
	} else if (type == QVariant::Pixmap) {
		QLabel *d = new QLabel();
		d->setPixmap(item.data().value<QPixmap>());
		addDataWidget(d, item.name());
		return;
	} else if (type == QVariant::Bool) {
		addLabel(item.data().toBool() ?
				 QT_TRANSLATE_NOOP("ContactInfo", "yes") :
				 QT_TRANSLATE_NOOP("ContactInfo", "no"),
				 item.name());
		return;
	} else if (item.data().canConvert<QHostAddress>()) {
		QHostAddress address = item.data().value<QHostAddress>();
		if (!address.isNull()) {
			addLabel(address.toString(), item.name());
			return;
		}
	} else if (type == QVariant::StringList) {
		QStringList list = item.data().toStringList();
		if (!list.isEmpty()) {
			foreach (const QString &i, list)
				addLabel(i, item.name());
			return;
		}
	}
	QString str;
	if (enabled) {
		str = item.data().toString();
		enabled = !str.isEmpty();
	}
	if (!enabled)
		str = QT_TRANSLATE_NOOP("ContactInfo", "the field is not set");
	addLabel(str, item.name())->setEnabled(enabled);
}

void ReadOnlyInfoLayout::addDataWidget(QWidget *widget, const QString &name)
{
	widget->setParent(parentWidget());
	widget->setObjectName(name);
	widget->setProperty("readOnly", true);
	addWidget(widget, m_row++, 1, 1, 1, Qt::AlignLeft);
}

QLabel *ReadOnlyInfoLayout::addLabel(const QString &data, const QString &name)
{
	QLabel *d = new QLabel(data, parentWidget());
	d->setTextInteractionFlags(Qt::LinksAccessibleByMouse |
							Qt::LinksAccessibleByKeyboard |
							Qt::TextSelectableByMouse |
							Qt::TextSelectableByKeyboard);
	addDataWidget(d, name);
	return d;
}

}

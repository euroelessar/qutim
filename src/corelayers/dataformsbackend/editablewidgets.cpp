#include "editablewidgets.h"
#include <libqutim/icon.h>
#include <QFileDialog>
#include "abstractdatalayout.h"

Q_DECLARE_METATYPE(QList<QIcon>);
Q_DECLARE_METATYPE(QList<QPixmap>);
Q_DECLARE_METATYPE(QList<QImage>);

namespace Core
{

using namespace qutim_sdk_0_3;

CheckBox::CheckBox(const DataItem &item)
{
	setText(item.title());
	setChecked(item.data().toBool());
}

DataItem CheckBox::item() const
{
	bool val = isChecked();
	QVariant d;
	if (val)
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

ComboBox::ComboBox(const QString &value, const LocalizedStringList &alt, const DataItem &item)
{
	int current = -1;
	int i = 0;
	addItem(notSpecifiedStr);
	foreach (const LocalizedString &str, alt) {
		if (value == str)
			current = i;
		addItem(str);
		++i;
	}
	setCurrentIndex(current + 1);
}

DataItem ComboBox::item() const
{
	QString val = currentText();
	QVariant d;
	if (!val.isEmpty() && val != notSpecifiedStr)
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

DateTimeEdit::DateTimeEdit(const DataItem &item)
{
	setDate(item.data().toDate());
}

DataItem DateTimeEdit::item() const
{
	QDateTime val = dateTime();
	QVariant d;
	if (val.isValid())
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

DateEdit::DateEdit(const DataItem &item)
{
	setDateTime(item.data().toDateTime());
}

DataItem DateEdit::item() const
{
	QDate val = date();
	QVariant d;
	if (val.isValid())
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

TextEdit::TextEdit(const DataItem &item)
{
	QString str;
	if (item.data().canConvert<LocalizedString>())
		str = item.data().value<LocalizedString>();
	else
		str = item.data().toString();
	setText(str);
}

DataItem TextEdit::item() const
{
	QString val = toPlainText();
	QVariant d;
	if (!val.isEmpty())
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

LineEdit::LineEdit(const DataItem &item)
{
	QString str;
	if (item.data().canConvert<LocalizedString>())
		str = item.data().value<LocalizedString>();
	else
		str = item.data().toString();
	setText(str);
}

DataItem LineEdit::item() const
{
	QString val = text();
	QVariant d;
	if (!val.isEmpty())
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

SpinBox::SpinBox(const DataItem &item)
{
	setValue(item.data().toInt());
}

DataItem SpinBox::item() const
{
	int val = value();
	QVariant d;
	if (val != 0)
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

DoubleSpinBox::DoubleSpinBox(const DataItem &item)
{
	setValue(item.data().toDouble());
}

DataItem DoubleSpinBox::item() const
{
	double val = value();
	QVariant d;
	if (val != 0)
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

IconListWidget::IconListWidget(const DataItem &item)
{
	setViewMode(IconMode);
	QSize size = item.property("imageSize", QSize(128, 128));
	QVariant altVariant = item.property("alternatives");
	QPixmap pixmap;
	quint64 cacheKey;
	QList<QPixmap> alt;
	QList<quint64> altCacheKeys;
	QVariant data = item.data();
	int type = data.type();
	if (type == QVariant::Icon) {
		QIcon icon = data.value<QIcon>();
		cacheKey = icon.cacheKey();
		pixmap = icon.pixmap(size);
		foreach (const QIcon &val, altVariant.value<QList<QIcon> >()) {
			altCacheKeys << val.cacheKey();
			alt << val.pixmap(size);
		}
	} else if (type == QVariant::Pixmap) {
		pixmap = data.value<QPixmap>();
		cacheKey = pixmap.cacheKey();
		if (!pixmap.isNull())
			pixmap = pixmap.scaled(size, Qt::KeepAspectRatio);
		foreach (const QPixmap &val, altVariant.value<QList<QPixmap> >()) {
			altCacheKeys << val.cacheKey();
			alt << val.scaled(size, Qt::KeepAspectRatio);
		}
	} else if (type == QVariant::Image) {
		QImage image = data.value<QImage>();
		cacheKey = image.cacheKey();
		pixmap = QPixmap::fromImage(image);
		if (!pixmap.isNull())
			pixmap = pixmap.scaled(size, Qt::KeepAspectRatio);
		foreach (const QImage &val, altVariant.value<QList<QImage> >()) {
			altCacheKeys << val.cacheKey();
			alt << QPixmap::fromImage(val).scaled(size, Qt::KeepAspectRatio);
		}
	}
	QListWidgetItem *currentItem = 0;
	foreach(const QIcon &icon, alt) {
		QListWidgetItem *tmp = new QListWidgetItem(this);
		tmp->setIcon(icon);
		addItem(tmp);
		quint64 altCacheKey = altCacheKeys.takeFirst();
		if (currentItem == 0 && cacheKey == altCacheKey)
			currentItem = tmp;
	}
	if (currentItem)
		setCurrentItem(currentItem);
}

DataItem IconListWidget::item() const
{
	QListWidgetItem *current = currentItem();
	QIcon val = current ? currentItem()->icon() : QIcon();
	QVariant d;
	if (!val.isNull())
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

IconWidget::IconWidget(const DataItem &item)
{
	m_size = item.property("imageSize", QSize(128, 128));
	QPixmap pixmap = variantToPixmap(item.data(), m_size);
	m_default = variantToPixmap(item.property("defaultImage"), m_size);
	if (m_default.isNull())
		m_default = pixmap;
	QGridLayout *layout = new QGridLayout(this);
	m_pixmapWidget = new QLabel(this);
	m_pixmapWidget->setFrameShape(QFrame::Panel);
	m_pixmapWidget->setFrameShadow(QFrame::Sunken);
	m_pixmapWidget->setAlignment(Qt::AlignCenter);
	m_pixmapWidget->setPixmap(pixmap.isNull() ? m_default : pixmap);
	QPushButton *setButton = new QPushButton(QIcon(), QString(), this);
	setButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setButton->setIcon(Icon("list-add"));
	connect(setButton, SIGNAL(clicked()), SLOT(setIcon()));
	QPushButton *removeButton = new QPushButton(QIcon(), QString(), this);
	connect(removeButton, SIGNAL(clicked()), SLOT(removeIcon()));
	removeButton->setIcon(Icon("list-remove"));
	layout->addWidget(m_pixmapWidget, 0, 0, 3, 1);
	layout->addWidget(setButton, 0, 1);
	layout->addItem(new QSpacerItem(1, 1, QSizePolicy::Preferred, QSizePolicy::Expanding), 2, 1);
	layout->addWidget(removeButton, 2, 1, Qt::AlignBottom);
}

DataItem IconWidget::item() const
{
	QVariant d;
	if (!m_path.isEmpty())	{
		if (m_type == QVariant::Icon)
			d.fromValue(QIcon(m_path));
		else if (m_type == QVariant::Pixmap)
			d.fromValue(QPixmap(m_path));
		else if (m_type == QVariant::Image)
			d.fromValue(QImage(m_path));
	}
	DataItem item(objectName(), LocalizedString(), d);
	item.setProperty("imagePath", m_path);
	return item;
}

void IconWidget::setIcon()
{
	m_path = QFileDialog::getOpenFileName(
			this,
			QT_TRANSLATE_NOOP("DataForms", "Open image"),
			QDir::homePath(),
			QT_TRANSLATE_NOOP("DataForms",
							  "Images (*.gif *.bmp *.jpg *.jpeg *.png);;"
							  "All files (*.*)"));
	if (!m_path.isEmpty())
		m_pixmapWidget->setPixmap(QPixmap(m_path).scaled(m_size, Qt::KeepAspectRatio));
	else
		removeIcon();
}

void IconWidget::removeIcon()
{
	m_pixmapWidget->setPixmap(m_default);
	m_path.clear();
}

}

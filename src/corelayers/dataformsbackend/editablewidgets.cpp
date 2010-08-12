#include "editablewidgets.h"
#include <qutim/icon.h>
#include <QFileDialog>
#include <QRegExpValidator>
#include "abstractdatalayout.h"

Q_DECLARE_METATYPE(QList<QIcon>);
Q_DECLARE_METATYPE(QList<QPixmap>);
Q_DECLARE_METATYPE(QList<QImage>);
Q_DECLARE_METATYPE(QLineEdit::EchoMode);
Q_DECLARE_METATYPE(QValidator*);

namespace Core
{

using namespace qutim_sdk_0_3;

static QValidator *getValidator(const QVariant &validator, QWidget *object)
{
	if (!validator.isNull()) {
		QValidator *d;
		if (validator.canConvert<QValidator*>())
			d = validator.value<QValidator*>();
		else
			d = new QRegExpValidator(validator.toRegExp(), object);
		return d;
	}
	return 0;
}

CheckBox::CheckBox(const DataItem &item)
{
	setText(item.title());
	setChecked(item.data().toBool());
}

DataItem CheckBox::item() const
{
	return DataItem(objectName(), LocalizedString(), isChecked());
}

ComboBox::ComboBox(const QString &value, const QStringList &alt,
				   const char *validatorProperty, const DataItem &item)
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
	QValidator *validator = getValidator(item.property(validatorProperty), this);
	if (validator)
		setValidator(validator);
	setEditable(item.property("editable", false));
	setMinimumContentsLength(12);
	setSizeAdjustPolicy(AdjustToMinimumContentsLength);
}

DataItem ComboBox::item() const
{
	QString val = currentText();
	QVariant d = (!val.isEmpty() && val != notSpecifiedStr) ? val : QString();
	return DataItem(objectName(), LocalizedString(), d);
}

DateTimeEdit::DateTimeEdit(const DataItem &item)
{
	setDateTime(item.data().toDateTime());
}

DataItem DateTimeEdit::item() const
{
	QDateTime val = dateTime();
	QVariant d = val.isValid() ? val : QDateTime();
	return DataItem(objectName(), LocalizedString(), d);
}

DateEdit::DateEdit(const DataItem &item)
{
	setDateTime(item.data().toDateTime());
}

DataItem DateEdit::item() const
{
	QDate val = date();
	QVariant d = val.isValid() ? val : QDate();
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
	QVariant d = !val.isEmpty() ? val : QString();
	return DataItem(objectName(), LocalizedString(), d);
}

LineEdit::LineEdit(const DataItem &item, const QString &textHint)
{
	QString str;
	if (textHint.isEmpty()) {
		if (item.data().canConvert<LocalizedString>())
			str = item.data().value<LocalizedString>();
		else
			str = item.data().toString();
	} else {
		str = textHint;
	}
	setText(str);
	QVariant passwordMode = item.property("passwordMode");
	if (!passwordMode.isNull()) {
		EchoMode mode = Normal;
		if (passwordMode.type() == QVariant::Bool)
			mode = Password;
		else if (passwordMode.canConvert<EchoMode>())
			mode = passwordMode.value<EchoMode>();
		else if (passwordMode.canConvert(QVariant::Int))
			mode = static_cast<EchoMode>(passwordMode.toInt());
		setEchoMode(mode);
	}
	QValidator *validator = getValidator(item.property("validator"), this);
	if (validator)
		setValidator(validator);
}

DataItem LineEdit::item() const
{
	QString val = text();
	QVariant d = !val.isEmpty() ? val : QString();
	return DataItem(objectName(), LocalizedString(), d);
}

SpinBox::SpinBox(const DataItem &item)
{
	setValue(item.data().toInt());
	bool ok;
	int value = item.property("maxValue").toInt(&ok);
	if (ok)
		setMaximum(value);
	value = item.property("minValue").toInt(&ok);
	if (ok)
		setMinimum(value);
}

DataItem SpinBox::item() const
{
	return DataItem(objectName(), LocalizedString(), value());
}

DoubleSpinBox::DoubleSpinBox(const DataItem &item)
{
	setValue(item.data().toDouble());
	bool ok;
	int value = item.property("maxValue").toDouble(&ok);
	if (ok)
		setMaximum(value);
	value = item.property("minValue").toDouble(&ok);
	if (ok)
		setMinimum(value);
}

DataItem DoubleSpinBox::item() const
{
	return DataItem(objectName(), LocalizedString(), value());
}

IconListWidget::IconListWidget(const DataItem &item)
{
	setViewMode(IconMode);
	QSize size = item.property("imageSize", QSize(32, 32));
	QVariant altVariant = item.property("alternatives");
	QPixmap pixmap;
	quint64 cacheKey = 0;
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
	QVariant d = !val.isNull() ? val : QIcon();
	return DataItem(objectName(), LocalizedString(), d);
}

IconWidget::IconWidget(const DataItem &item)
{
	m_size = item.property("imageSize", QSize(32, 32));
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

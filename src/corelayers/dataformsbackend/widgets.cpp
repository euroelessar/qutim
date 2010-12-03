#include "widgets.h"
#include <qutim/icon.h>
#include <QFileDialog>
#include <QRegExpValidator>
#include "datalayout.h"
#include "widgetgenerator.h"

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

Label::Label(const DataItem &item, QWidget *parent) :
	QLabel(parent), m_item(item)
{
	setTextInteractionFlags(Qt::LinksAccessibleByMouse |
							   Qt::LinksAccessibleByKeyboard |
							   Qt::TextSelectableByMouse |
							   Qt::TextSelectableByKeyboard);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
	setWordWrap(true);

	bool enabled = true;
	QVariant::Type type = item.data().type();
	QString value;
	if (item.property("notSet", false)) {
		enabled = false;
	} else if (type == QVariant::Date) {
		value = item.data().toDate().toString(Qt::SystemLocaleLongDate);
	} else if (type == QVariant::DateTime) {
		value = item.data().toDateTime().toString(Qt::SystemLocaleLongDate);
	} else if (type == QVariant::Icon || type == QVariant::Pixmap || type == QVariant::Image) {
		QSize size = item.property("imageSize", QSize(128, 128));
		QPixmap pixmap = variantToPixmap(item.data(), size);
		if (pixmap.isNull())
			pixmap = variantToPixmap(item.property("defaultImage"), size);
		setPixmap(pixmap);
		setFrameShape(QFrame::Panel);
		setFrameShadow(QFrame::Sunken);
		setAlignment(Qt::AlignCenter);
		return;
	} else if (type == QVariant::Bool) {
		value = item.data().toBool() ?
				QT_TRANSLATE_NOOP("DataForms", "yes") :
				QT_TRANSLATE_NOOP("DataForms", "no");
	} else if (item.data().canConvert<QHostAddress>()) {
		QHostAddress address = item.data().value<QHostAddress>();
		if (!address.isNull())
			value = address.toString();
	} else if (type == QVariant::StringList) {
		value = item.data().toStringList().join("<br>");
	} else if (item.data().canConvert<LocalizedString>()) {
		value = item.data().value<LocalizedString>();
	} else if (item.data().canConvert<LocalizedStringList>()) {
		bool first = true;
		foreach (const LocalizedString &str, item.data().value<LocalizedStringList>()) {
			if (!first)
				first = false;
			else
				value += "<br>";
			value += str;
		}
	}
	if (enabled) {
		if (value.isEmpty())
			value = item.data().toString();
		enabled = !value.isEmpty();
	}
	if (!enabled)
		value = QT_TRANSLATE_NOOP("DataForms", "the field is not set");
	setText(value);
	setEnabled(enabled);
}

DataItem Label::item() const
{
	return m_item;
}

CheckBox::CheckBox(const DataItem &item, QWidget *parent) :
	QCheckBox(parent)
{
	setText(item.title());
	setChecked(item.data().toBool());
}

DataItem CheckBox::item() const
{
	return DataItem(objectName(), LocalizedString(), isChecked());
}

ComboBox::ComboBox(const QString &value, const QStringList &alt,
				   const char *validatorProperty, const DataItem &item,
				   QWidget *parent) :
	QComboBox(parent)
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

DateTimeEdit::DateTimeEdit(const DataItem &item, QWidget *parent) :
	QDateTimeEdit(parent)
{
	setDateTime(item.data().toDateTime());
}

DataItem DateTimeEdit::item() const
{
	QDateTime val = dateTime();
	QVariant d = val.isValid() ? val : QDateTime();
	return DataItem(objectName(), LocalizedString(), d);
}

DateEdit::DateEdit(const DataItem &item, QWidget *parent) :
	QDateEdit(parent)
{
	setDateTime(item.data().toDateTime());
}

DataItem DateEdit::item() const
{
	QDate val = date();
	QVariant d = val.isValid() ? val : QDate();
	return DataItem(objectName(), LocalizedString(), d);
}

TextEdit::TextEdit(const DataItem &item, QWidget *parent) :
	QTextEdit(parent)
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

LineEdit::LineEdit(const DataItem &item, const QString &textHint, QWidget *parent) :
	QLineEdit(parent)
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

SpinBox::SpinBox(const DataItem &item, QWidget *parent) :
	QSpinBox(parent)
{
	bool ok;
	int value = item.property("maxValue").toInt(&ok);
	if (ok)
		setMaximum(value);
	value = item.property("minValue").toInt(&ok);
	if (ok)
		setMinimum(value);
	setValue(item.data().toInt());
}

DataItem SpinBox::item() const
{
	return DataItem(objectName(), LocalizedString(), value());
}

DoubleSpinBox::DoubleSpinBox(const DataItem &item, QWidget *parent) :
	QDoubleSpinBox(parent)
{
	bool ok;
	int value = item.property("maxValue").toDouble(&ok);
	if (ok)
		setMaximum(value);
	value = item.property("minValue").toDouble(&ok);
	if (ok)
		setMinimum(value);
	setValue(item.data().toDouble());
}

DataItem DoubleSpinBox::item() const
{
	return DataItem(objectName(), LocalizedString(), value());
}

IconListWidget::IconListWidget(const DataItem &item, QWidget *parent) :
	QListWidget(parent)
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

IconWidget::IconWidget(const DataItem &item, QWidget *parent) :
	QWidget(parent)
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

ModifiableGroup::ModifiableGroup(const DataItem &item, QWidget *parent) :
	QGroupBox(parent)
{
	setObjectName(item.name());
	setTitle(item.title());
	QVBoxLayout *layout = new QVBoxLayout(this);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_widget = new ModifiableWidget(item, this);
	layout->addWidget(m_widget);
}

DataItem ModifiableGroup::item() const
{
	DataItem item = m_widget->item();
	item.setName(objectName());
	return item;
}

DataGroup::DataGroup(const DataItem &items, bool editable, QWidget *parent) :
	QGroupBox(parent)
{
	setTitle(items.title());
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_layout = new DataLayout(this);
	m_layout->addItems(items.subitems());
}

DataItem DataGroup::item() const
{
	DataItem item = m_layout->item();
	item.setName(objectName());
	return item;
}

StringListGroup::StringListGroup(const DataItem &item, QWidget *parent) :
	ModifiableWidget(parent)
{
	m_max = item.property("maxStringsCount", -1);
	m_def = item;
	m_def.setData(QVariant(QVariant::String));
	m_def.setProperty("hideTitle", true);
	m_def.allowModifySubitems(DataItem(), 1);
	QStringList alt = variantToStringList(item.property("alternatives"));
	foreach (const QString &str, variantToStringList(item.data())) {
		if (!alt.isEmpty())
			addRow(new ComboBox(str, alt, "validator", item));
		else
			addRow(new LineEdit(item, str));
	}
}

DataItem StringListGroup::item() const
{
	QStringList list;
	foreach (const WidgetLine &line, m_widgets)
		list << getDataItem(line.title, line.data).data().toString();
	DataItem item;
	item.setName(objectName());
	item.setData(list);
	return item;
}

}

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

static inline void connectSignalsHelper(QWidget *widget, DefaultDataForm *dataForm,
										const DataItem &item, const char *dataChangedSignal)
{
	QObject::connect(widget, dataChangedSignal, dataForm, SLOT(dataChanged()));
	if (item.dataChangedReceiver()) {
		Q_ASSERT(item.dataChangedMethod());
		QObject::connect(widget, dataChangedSignal, widget, SLOT(onChanged()));
		QObject::connect(widget, SIGNAL(changed(QVariant)),
						 item.dataChangedReceiver(), item.dataChangedMethod());
	}
}

Label::Label(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QLabel(parent), m_item(item)
{
	Q_UNUSED(dataForm);
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

CheckBox::CheckBox(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QCheckBox(parent)
{
	setText(item.title());
	setChecked(item.data().toBool());
	connectSignalsHelper(this, dataForm, item, SIGNAL(stateChanged(int)));
}

DataItem CheckBox::item() const
{
	return DataItem(objectName(), LocalizedString(), isChecked());
}

void CheckBox::onChanged()
{
	emit changed(isChecked());
}

ComboBox::ComboBox(DefaultDataForm *dataForm,
				   const QString &value, const QStringList &alt,
				   const char *validatorProperty, const DataItem &item,
				   QWidget *parent) :
	QComboBox(parent)
{
	int current = -1;
	int i = 0;
	foreach (const LocalizedString &str, alt) {
		if (value == str)
			current = i;
		addItem(str);
		++i;
	}
	setCurrentIndex(current);
	QValidator *validator = getValidator(item.property(validatorProperty), this);
	if (validator)
		setValidator(validator);
	setEditable(item.property("editable", false));
	setMinimumContentsLength(12);
	setSizeAdjustPolicy(AdjustToMinimumContentsLength);
	connectSignalsHelper(this, dataForm, item, SIGNAL(currentIndexChanged(int)));
}

DataItem ComboBox::item() const
{
	return DataItem(objectName(), LocalizedString(), currentText());
}

void ComboBox::onChanged()
{
	emit changed(currentText());
}

DateTimeEdit::DateTimeEdit(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QDateTimeEdit(parent)
{
	setDateTime(item.data().toDateTime());
	connectSignalsHelper(this, dataForm, item, SIGNAL(dateTimeChanged(QDateTime)));
}

DataItem DateTimeEdit::item() const
{
	return DataItem(objectName(), LocalizedString(), data());
}

inline QVariant DateTimeEdit::data() const
{
	QDateTime val = dateTime();
	return val.isValid() ? val : QDateTime();
}

void DateTimeEdit::onChanged()
{
	emit changed(data());
}

DateEdit::DateEdit(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QDateEdit(parent)
{
	setDateTime(item.data().toDateTime());
	connectSignalsHelper(this, dataForm, item, SIGNAL(dateChanged(QDate)));
}

DataItem DateEdit::item() const
{
	return DataItem(objectName(), LocalizedString(), data());
}

inline QVariant DateEdit::data() const
{
	QDate val = date();
	return val.isValid() ? val : QDate();
}

void DateEdit::onChanged()
{
	emit changed(data());
}

TextEdit::TextEdit(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QTextEdit(parent)
{
	QString str;
	if (item.data().canConvert<LocalizedString>())
		str = item.data().value<LocalizedString>();
	else
		str = item.data().toString();
	setText(str);
	connectSignalsHelper(this, dataForm, item, SIGNAL(textChanged()));
}

DataItem TextEdit::item() const
{
	return DataItem(objectName(), LocalizedString(), data());
}

inline QVariant TextEdit::data() const
{
	QString val = toPlainText();
	return !val.isEmpty() ? val : QString();
}

void TextEdit::onChanged()
{
	emit changed(data());
}

LineEdit::LineEdit(DefaultDataForm *dataForm, const DataItem &item, const QString &textHint, QWidget *parent) :
	QLineEdit(parent), m_dataForm(dataForm), m_complete(true)
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
	QVariant isPassword = item.property("password");
	if (!isPassword.isNull()) {
		EchoMode mode = Normal;
		if (isPassword.type() == QVariant::Bool) {
			if (isPassword.toBool())
				mode = Password;
		} else if (isPassword.canConvert<EchoMode>()) {
			mode = isPassword.value<EchoMode>();
		} else if (isPassword.canConvert(QVariant::Int)) {
			mode = static_cast<EchoMode>(isPassword.toInt());
		}
		setEchoMode(mode);
	}
	QValidator *validator = getValidator(item.property("validator"), this);
	if (validator)
		setValidator(validator);
	m_mandatory = item.property("mandatory", false);
	updateCompleteState(str);

	connect(this, SIGNAL(textChanged(QString)), SLOT(textChanged(QString)));
	m_emitChangedSignal = item.dataChangedReceiver();
	if (m_emitChangedSignal) {
		Q_ASSERT(item.dataChangedMethod());
		connect(this, SIGNAL(changed(QVariant)),
				item.dataChangedReceiver(), item.dataChangedMethod());
	}
}

DataItem LineEdit::item() const
{
	return DataItem(objectName(), LocalizedString(), data());
}

inline QVariant LineEdit::data() const
{
	QString val = text();
	return !val.isEmpty() ? val : QString();
}

void LineEdit::textChanged(const QString &text)
{
	m_dataForm->dataChanged();
	updateCompleteState(text);
	if (m_emitChangedSignal)
		emit changed(data());
}

void LineEdit::updateCompleteState(const QString &text)
{
	bool isComplete = hasAcceptableInput();
	if (m_mandatory)
		isComplete = isComplete && !text.isEmpty();
	if (isComplete != m_complete) {
		m_complete = isComplete;
		m_dataForm->completeChanged(m_complete);
	}
}

SpinBox::SpinBox(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
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
	connectSignalsHelper(this, dataForm, item, SIGNAL(valueChanged(int)));
}

DataItem SpinBox::item() const
{
	return DataItem(objectName(), LocalizedString(), value());
}

void SpinBox::onChanged()
{
	emit changed(value());
}

DoubleSpinBox::DoubleSpinBox(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
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
	connectSignalsHelper(this, dataForm, item, SIGNAL(valueChanged(double)));
}

DataItem DoubleSpinBox::item() const
{
	return DataItem(objectName(), LocalizedString(), value());
}

void DoubleSpinBox::onChanged()
{
	emit changed(value());
}

IconListWidget::IconListWidget(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
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
	connectSignalsHelper(this, dataForm, item,
						 SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)));
}

DataItem IconListWidget::item() const
{
	return DataItem(objectName(), LocalizedString(), data());
}

inline QVariant IconListWidget::data() const
{
	QListWidgetItem *current = currentItem();
	QIcon val = current ? currentItem()->icon() : QIcon();
	return !val.isNull() ? val : QIcon();
}

void IconListWidget::onChanged()
{
	emit changed(data());
}

IconWidget::IconWidget(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QWidget(parent), m_dataForm(dataForm)
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

	m_emitChangedSignal = item.dataChangedReceiver();
	if (m_emitChangedSignal) {
		Q_ASSERT(item.dataChangedMethod());
		connect(this, SIGNAL(changed(QVariant)),
				item.dataChangedReceiver(), item.dataChangedMethod());
	}
}

DataItem IconWidget::item() const
{
	DataItem item(objectName(), LocalizedString(), data());
	item.setProperty("imagePath", m_path);
	return item;
}

inline QVariant IconWidget::data() const
{
	if (!m_path.isEmpty())	{
		if (m_type == QVariant::Icon)
			return QVariant::fromValue(QIcon(m_path));
		else if (m_type == QVariant::Pixmap)
			return QVariant::fromValue(QPixmap(m_path));
		else if (m_type == QVariant::Image)
			return QVariant::fromValue(QImage(m_path));
	}
	return QVariant();
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
	if (!m_path.isEmpty()) {
		m_pixmapWidget->setPixmap(QPixmap(m_path).scaled(m_size, Qt::KeepAspectRatio));
		onDataChanged();
	} else {
		removeIcon();
	}
}

void IconWidget::removeIcon()
{
	m_pixmapWidget->setPixmap(m_default);
	m_path.clear();
	onDataChanged();
}

void IconWidget::onDataChanged()
{
	m_dataForm->dataChanged();
	if (m_emitChangedSignal)
		emit changed(data());
}

ModifiableGroup::ModifiableGroup(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QGroupBox(parent)
{
	setObjectName(item.name());
	setTitle(item.title());
	QVBoxLayout *layout = new QVBoxLayout(this);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_widget = new ModifiableWidget(item, dataForm, this);
	layout->addWidget(m_widget);
}

DataItem ModifiableGroup::item() const
{
	DataItem item = m_widget->item();
	item.setName(objectName());
	return item;
}

DataGroup::DataGroup(DefaultDataForm *dataForm, const DataItem &items, QWidget *parent) :
	QGroupBox(parent)
{
	setTitle(items.title());
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_layout = new DataLayout(dataForm, this);
	m_layout->addItems(items.subitems());
}

DataItem DataGroup::item() const
{
	DataItem item = m_layout->item();
	item.setName(objectName());
	return item;
}

StringListGroup::StringListGroup(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	ModifiableWidget(dataForm, parent)
{
	m_max = item.property("maxStringsCount", -1);
	m_def = item;
	m_def.setData(QVariant(QVariant::String));
	m_def.setProperty("hideTitle", true);
	m_def.allowModifySubitems(DataItem(), 1);
	QStringList alt = variantToStringList(item.property("alternatives"));
	foreach (const QString &str, variantToStringList(item.data())) {
		if (!alt.isEmpty())
			addRow(new ComboBox(dataForm, str, alt, "validator", item));
		else
			addRow(new LineEdit(dataForm, item, str));
	}

	if (item.dataChangedReceiver()) {
		Q_ASSERT(item.dataChangedMethod());
		connect(this, SIGNAL(changed(QVariant)),
				item.dataChangedReceiver(), item.dataChangedMethod());
		connect(this, SIGNAL(rowAdded()), SLOT(onChanged()));
		connect(this, SIGNAL(rowRemoved()), SLOT(onChanged()));
	}
}

DataItem StringListGroup::item() const
{
	DataItem item;
	item.setName(objectName());
	item.setData(data());
	return item;
}

QVariant StringListGroup::data() const
{
	QStringList list;
	foreach (const WidgetLine &line, m_widgets)
		list << getDataItem(line.title, line.data).data().toString();
	return list;
}

void StringListGroup::onChanged()
{
	emit changed(data());
}

}

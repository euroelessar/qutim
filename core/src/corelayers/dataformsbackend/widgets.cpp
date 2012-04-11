/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#include "widgets.h"
#include <qutim/icon.h>
#include <QFileDialog>
#include <QRegExpValidator>
#include "datalayout.h"
#include "widgetgenerator.h"
#include <qutim/debug.h>

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
	Q_ASSERT(qobject_cast<AbstractDataWidget*>(widget));
	if (!item.name().isEmpty())
		dataForm->addWidget(item.name(), reinterpret_cast<AbstractDataWidget*>(widget));
	QObject::connect(widget, dataChangedSignal, dataForm, SLOT(dataChanged()));
	if (item.dataChangedReceiver()) {
		Q_ASSERT(item.dataChangedMethod());
		QObject::connect(widget, dataChangedSignal, widget, SLOT(onChanged()));
		QObject::connect(widget, SIGNAL(changed(QString,QVariant,qutim_sdk_0_3::AbstractDataForm*)),
						 item.dataChangedReceiver(), item.dataChangedMethod());
	}
}

Label::Label(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QLabel(parent), AbstractDataWidget(item, dataForm)
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
	QCheckBox(parent), AbstractDataWidget(item, dataForm)
{
	setText(item.title());
	setChecked(item.data().toBool());
	connectSignalsHelper(this, dataForm, item, SIGNAL(stateChanged(int)));
}

DataItem CheckBox::item() const
{
	DataItem item = m_item;
	item.setName(objectName());
	item.setData(isChecked());
	return item;
}

void CheckBox::setData(const QVariant &data)
{
	setChecked(data.toBool());
}

void CheckBox::onChanged()
{
	emit changed(objectName(), isChecked(), dataForm());
}

ComboBox::ComboBox(DefaultDataForm *dataForm,
				   const QString &value, const QStringList &alt,
				   bool isTitle, const DataItem &item,
				   QWidget *parent) :
	QComboBox(parent), AbstractDataWidget(item, dataForm), m_complete(true)
{
	int current = -1;
	QVariantList ids = item.property(isTitle ? "titleIdentificators" : "identificators", QVariantList());
	for (int i = 0; i < alt.size(); i++) {
		QString str = alt.at(i);
		if (value == str)
			current = i;
		addItem(str, ids.value(i));
	}
	debug() << Q_FUNC_INFO << value << alt << count();

	setEditable(item.property("editable", false));
	setMinimumContentsLength(12);
	setSizeAdjustPolicy(AdjustToMinimumContentsLength);
	setCurrentIndex(current);

	QVariant validatorVar = item.property(isTitle ? "titleValidator" : "validator");
	QValidator *validator = getValidator(validatorVar, this);
	if (validator)
		setValidator(validator);
	m_mandatory = item.property("mandatory", false);
	QString str = currentText();
	updateCompleteState(str);

	if (!item.name().isEmpty())
		dataForm->addWidget(item.name(), this);
	connect(this, SIGNAL(editTextChanged(QString)), SLOT(onChanged(QString)));
	connect(this, SIGNAL(currentIndexChanged(QString)), SLOT(onChanged(QString)));
	m_emitChangedSignal = item.dataChangedReceiver();
	if (m_emitChangedSignal) {
		Q_ASSERT(item.dataChangedMethod());
		connect(this, SIGNAL(changed(QString,QVariant,qutim_sdk_0_3::AbstractDataForm*)),
				item.dataChangedReceiver(), item.dataChangedMethod());
	}
}

DataItem ComboBox::item() const
{
	DataItem item = m_item;
	item.setName(objectName());
	debug() << Q_FUNC_INFO << currentText();
	item.setData(currentText());
	item.setProperty("identificator", itemData(currentIndex()));
	return item;
}

void ComboBox::setData(const QVariant &data)
{
	const QString &str = data.toString();
	int index = findText(str);
	if (index != -1) {
		setCurrentIndex(index);
	} else {
		if (isEditable())
			setEditText(str);
	}
}

void ComboBox::onChanged(QString text)
{
	dataForm()->dataChanged();
	updateCompleteState(text);
	if (m_emitChangedSignal)
		emit changed(objectName(), currentText(), dataForm());
}

void ComboBox::updateCompleteState(QString &text)
{
	int pos = 0;
	bool isComplete = validator() ?
					  validator()->validate(text, pos) == QValidator::Acceptable :
					  true;
	if (m_mandatory)
		isComplete = isComplete && !text.isEmpty();
	if (isComplete != m_complete) {
		m_complete = isComplete;
		dataForm()->completeChanged(m_complete);
	}
}

DateTimeEdit::DateTimeEdit(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QDateTimeEdit(parent), AbstractDataWidget(item, dataForm)
{
	setDateTime(item.data().toDateTime());
	setCalendarPopup(true);
	connectSignalsHelper(this, dataForm, item, SIGNAL(dateTimeChanged(QDateTime)));
}

DataItem DateTimeEdit::item() const
{
	DataItem item = m_item;
	item.setName(objectName());
	item.setData(data());
	return item;
}

void DateTimeEdit::setData(const QVariant &data)
{
	setDateTime(data.toDateTime());
}

inline QVariant DateTimeEdit::data() const
{
	QDateTime val = dateTime();
	return val.isValid() ? val : QDateTime();
}

void DateTimeEdit::onChanged()
{
	emit changed(objectName(),data(), dataForm());
}

DateEdit::DateEdit(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QDateEdit(parent), AbstractDataWidget(item, dataForm)
{
	setDate(item.data().toDate());
	setCalendarPopup(true);
	connectSignalsHelper(this, dataForm, item, SIGNAL(dateChanged(QDate)));
}

DataItem DateEdit::item() const
{
	DataItem item = m_item;
	item.setName(objectName());
	item.setData(data());
	return item;
}

void DateEdit::setData(const QVariant &data)
{
	setDate(data.toDate());
}

inline QVariant DateEdit::data() const
{
	QDate val = date();
	return val.isValid() ? val : QDate();
}

void DateEdit::onChanged()
{
	emit changed(objectName(), data(), dataForm());
}

TextEdit::TextEdit(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QTextEdit(parent), AbstractDataWidget(item, dataForm)
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
	DataItem item = m_item;
	item.setName(objectName());
	item.setData(data());
	return item;
}

inline QVariant TextEdit::data() const
{
	QString val = toPlainText();
	return !val.isEmpty() ? val : QString();
}

void TextEdit::onChanged()
{
	emit changed(objectName(), data(), dataForm());
}

LineEdit::LineEdit(DefaultDataForm *dataForm, const DataItem &item, const QString &textHint, QWidget *parent) :
	QLineEdit(parent), AbstractDataWidget(item, dataForm), m_complete(true)
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

	if (!item.name().isEmpty())
		dataForm->addWidget(item.name(), this);
	connect(this, SIGNAL(textChanged(QString)), SLOT(textChanged(QString)));
	m_emitChangedSignal = item.dataChangedReceiver();
	if (m_emitChangedSignal) {
		Q_ASSERT(item.dataChangedMethod());
		connect(this, SIGNAL(changed(QString,QVariant,qutim_sdk_0_3::AbstractDataForm*)),
				item.dataChangedReceiver(), item.dataChangedMethod());
	}
}

DataItem LineEdit::item() const
{
	DataItem item = m_item;
	item.setName(objectName());
	item.setData(data());
	return item;
}

void LineEdit::setData(const QVariant &data)
{
	setText(data.toString());
}

inline QVariant LineEdit::data() const
{
	QString val = text();
	return !val.isEmpty() ? val : QString();
}

void LineEdit::textChanged(const QString &text)
{
	dataForm()->dataChanged();
	updateCompleteState(text);
	if (m_emitChangedSignal)
		emit changed(objectName(), data(), dataForm());
}

void LineEdit::updateCompleteState(const QString &text)
{
	bool isComplete = hasAcceptableInput();
	if (m_mandatory)
		isComplete = isComplete && !text.isEmpty();
	if (isComplete != m_complete) {
		m_complete = isComplete;
		dataForm()->completeChanged(m_complete);
	}
}

SpinBox::SpinBox(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QSpinBox(parent), AbstractDataWidget(item, dataForm)
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
	DataItem item = m_item;
	item.setName(objectName());
	item.setData(value());
	return item;
}

void SpinBox::setData(const QVariant &data)
{
	setValue(data.toInt());
}

void SpinBox::onChanged()
{
	emit changed(objectName(), value(), dataForm());
}

DoubleSpinBox::DoubleSpinBox(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QDoubleSpinBox(parent), AbstractDataWidget(item, dataForm)
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
	DataItem item = m_item;
	item.setName(objectName());
	item.setData(value());
	return item;
}

void DoubleSpinBox::setData(const QVariant &data)
{
	setValue(data.toDouble());
}

void DoubleSpinBox::onChanged()
{
	emit changed(objectName(), value(), dataForm());
}

IconListWidget::IconListWidget(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QListWidget(parent), AbstractDataWidget(item, dataForm)
{
	setViewMode(IconMode);
	QSize size = item.property("imageSize", QSize(32, 32));
	QVariant altVariant = item.property("alternatives");
	QPixmap pixmap;
	quint64 cacheKey = 0;
	QList<QPixmap> alt;
	QList<quint64> altCacheKeys;
	QVariant data = item.data();
	m_type = data.type();
	if (m_type == QVariant::Icon) {
		QIcon icon = data.value<QIcon>();
		cacheKey = icon.cacheKey();
		pixmap = icon.pixmap(size);
		foreach (const QIcon &val, altVariant.value<QList<QIcon> >()) {
			altCacheKeys << val.cacheKey();
			alt << val.pixmap(size);
		}
	} else if (m_type == QVariant::Pixmap) {
		pixmap = data.value<QPixmap>();
		cacheKey = pixmap.cacheKey();
		if (!pixmap.isNull())
			pixmap = pixmap.scaled(size, Qt::KeepAspectRatio);
		foreach (const QPixmap &val, altVariant.value<QList<QPixmap> >()) {
			altCacheKeys << val.cacheKey();
			alt << val.scaled(size, Qt::KeepAspectRatio);
		}
	} else if (m_type == QVariant::Image) {
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
		m_items.insert(altCacheKey, tmp);
	}
	if (currentItem)
		setCurrentItem(currentItem);
	connectSignalsHelper(this, dataForm, item,
						 SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)));
}

DataItem IconListWidget::item() const
{
	DataItem item = m_item;
	item.setName(objectName());
	item.setData(data());
	return item;
}

void IconListWidget::setData(const QVariant &data)
{
	quint64 cacheKey = 0;
	if (m_type == QVariant::Icon)
		cacheKey = data.value<QIcon>().cacheKey();
	else if (m_type == QVariant::Pixmap)
		cacheKey = data.value<QPixmap>().cacheKey();
	else if (m_type == QVariant::Image)
		cacheKey = data.value<QImage>().cacheKey();
	if (cacheKey != 0 && m_items.contains(cacheKey))
		setCurrentItem(m_items.value(cacheKey));
}

inline QVariant IconListWidget::data() const
{
	QListWidgetItem *current = currentItem();
	QIcon val = current ? currentItem()->icon() : QIcon();
	return !val.isNull() ? val : QIcon();
}

void IconListWidget::onChanged()
{
	emit changed(objectName(), data(), dataForm());
}

IconWidget::IconWidget(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QWidget(parent), AbstractDataWidget(item, dataForm)
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
	m_path = item.property("imagePath", QString());
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

	if (!item.name().isEmpty())
		dataForm->addWidget(item.name(), this);
	m_emitChangedSignal = item.dataChangedReceiver();
	if (m_emitChangedSignal) {
		Q_ASSERT(item.dataChangedMethod());
		connect(this, SIGNAL(changed(QString,QVariant,qutim_sdk_0_3::AbstractDataForm*)),
				item.dataChangedReceiver(), item.dataChangedMethod());
	}
}

DataItem IconWidget::item() const
{
	DataItem item = m_item;
	item.setName(objectName());
	item.setData(data());
	item.setProperty("imagePath", m_path);
	return item;
}

void IconWidget::setData(const QVariant &data)
{
	if (data.canConvert(QVariant::String)) {
		m_path = data.toString();
		updatePixmap();
	} else {
		m_path.clear();
		m_pixmapWidget->setPixmap(variantToPixmap(data, m_size));
		onDataChanged();
	}
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
	} else {
		const QPixmap &pixmap = *m_pixmapWidget->pixmap();
		if (m_type == QVariant::Icon)
			return QVariant::fromValue(QIcon(pixmap));
		else if (m_type == QVariant::Pixmap)
			return QVariant::fromValue(pixmap);
		else if (m_type == QVariant::Image)
			return QVariant::fromValue(pixmap.toImage());
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
	updatePixmap();
}

void IconWidget::removeIcon()
{
	m_pixmapWidget->setPixmap(m_default);
	m_path.clear();
	onDataChanged();
}

void IconWidget::onDataChanged()
{
	dataForm()->dataChanged();
	if (m_emitChangedSignal)
		emit changed(objectName(), data(), dataForm());
}

void IconWidget::updatePixmap()
{
	if (!m_path.isEmpty()) {
		m_pixmapWidget->setPixmap(QPixmap(m_path).scaled(m_size, Qt::KeepAspectRatio));
		onDataChanged();
	} else {
		removeIcon();
	}
}

ModifiableGroup::ModifiableGroup(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	QGroupBox(parent), AbstractDataWidget(item, dataForm)
{
	setObjectName(item.name());
	if (!item.property("hideTitle", false))
		setTitle(item.title());
	QVBoxLayout *layout = new QVBoxLayout(this);
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
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
	QGroupBox(parent), AbstractDataWidget(items, dataForm)
{
	if (!items.property("hideTitle", false))
		setTitle(items.title());
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_layout = new DataLayout(items, dataForm, items.property<quint16>("columns", 1), this);
	m_layout->addDataItems(items.subitems());

	QVariant spacing = items.property("horizontalSpacing");
	if (spacing.canConvert(QVariant::Int))
		m_layout->setHorizontalSpacing(spacing.toInt());
	spacing = items.property("verticalSpacing");
	if (spacing.canConvert(QVariant::Int))
		m_layout->setVerticalSpacing(spacing.toInt());
}

DataItem DataGroup::item() const
{
	DataItem item = m_layout->item();
	item.setName(objectName());
	return item;
}

StringListGroup::StringListGroup(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent) :
	ModifiableWidget(item, dataForm, parent)
{
	m_max = item.property("maxStringsCount", -1);
	m_def = item;
	m_def.setData(QVariant(QVariant::String));
	m_def.setProperty("hideTitle", true);
	m_def.allowModifySubitems(DataItem(), 1);
	m_alt = variantToStringList(item.property("alternatives"));
	setData(item.data());

	if (!item.name().isEmpty())
		dataForm->addWidget(item.name(), this);
	if (item.dataChangedReceiver()) {
		Q_ASSERT(item.dataChangedMethod());
		connect(this, SIGNAL(changed(QString,QVariant,qutim_sdk_0_3::AbstractDataForm*)),
				item.dataChangedReceiver(), item.dataChangedMethod());
		connect(this, SIGNAL(rowAdded()), SLOT(onChanged()));
		connect(this, SIGNAL(rowRemoved()), SLOT(onChanged()));
	}
}

DataItem StringListGroup::item() const
{
	DataItem item = m_item;
	item.setName(objectName());
	item.setData(data());
	return item;
}

void StringListGroup::setData(const QVariant &data)
{
	foreach (const QString &str, variantToStringList(data)) {
		if (!m_alt.isEmpty())
			addRow(new ComboBox(dataForm(), str, m_alt, false, m_item));
		else
			addRow(new LineEdit(dataForm(), m_item, str));
	}
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
	emit changed(objectName(), data(), dataForm());
}

}


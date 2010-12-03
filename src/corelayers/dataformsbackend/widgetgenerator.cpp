#include "widgetgenerator.h"
#include "datalayout.h"
#include "widgets.h"
#include <QGroupBox>
#include <QLabel>
#include <QDate>
#include <QDateEdit>

namespace Core {

QPixmap variantToPixmap(const QVariant &data, const QSize &size)
{
	int type = data.type();
	QPixmap pixmap;
	if (type == QVariant::Icon)
		return data.value<QIcon>().pixmap(size); // The pixmap can be returned as its size is correct.
	else if (type == QVariant::Pixmap)
		pixmap = data.value<QPixmap>();
	else if (type == QVariant::Image)
		pixmap = QPixmap::fromImage(data.value<QImage>());
	if (!pixmap.isNull())
		return pixmap.scaled(size, Qt::KeepAspectRatio);
	else
		return pixmap;
}

QStringList variantToStringList(const QVariant &data)
{
	QStringList list = qVariantValue<QStringList>(data);
	if (list.isEmpty()) {
		foreach (const LocalizedString &str, qVariantValue<LocalizedStringList>(data))
			list << str;
	}
	return list;
}

QWidget *getTitle(const DataItem &item, const Qt::Alignment &alignment, QWidget *parent)
{
	QStringList alt = variantToStringList(item.property("titleAlternatives"));
	if (item.isReadOnly() || alt.isEmpty()) {
		QLabel *title = new QLabel(item.title() + ":",parent);
		title->setAlignment(alignment);
		if (item.isReadOnly())
			title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
		return title;
	} else {
		return new ComboBox(item.title(), alt, "validator", item);
	}
}

static QWidget *getWidgetHelper(const DataItem &item, bool *twoColumn, QSizePolicy::Policy &verticalPolicy, QWidget *parent)
{
	verticalPolicy = QSizePolicy::Maximum;
	if (twoColumn)
		*twoColumn = false;
	QVariant::Type type = item.data().type();
	if (item.property("readOnly", false)) {
		if (item.hasSubitems()) {
			if (twoColumn)
				*twoColumn = true;
			return new DataGroup(item, false, parent);
		} else {
			return new Label(item, parent);
		}
	} else if (type == QVariant::StringList || item.data().canConvert<LocalizedStringList>()) {
		return new StringListGroup(item, parent);
	} else if (item.isAllowedModifySubitems()) {
		if (twoColumn)
			*twoColumn = true;
		return new ModifiableGroup(item, parent);
	} else if (item.hasSubitems()) {
		if (twoColumn)
			*twoColumn = true;
		return new DataGroup(item, true, parent);
	} else if (type == QVariant::Bool) {
		if (twoColumn)
			*twoColumn = true;
		return new CheckBox(item, parent);
	} else if (type == QVariant::Date) {
		return new DateEdit(item, parent);
	} else if (type == QVariant::DateTime) {
		return new DateTimeEdit(item, parent);
	} else if (type == QVariant::Int || type == QVariant::LongLong || type == QVariant::UInt) {
		return new SpinBox(item, parent);
	} else if (type == QVariant::Double) {
		return new DoubleSpinBox(item, parent);
	} else if (type == QVariant::Icon || type == QVariant::Pixmap || type == QVariant::Image) {
		if (item.property("alternatives").isNull())
			return new IconWidget(item, parent);
		else
			return new IconListWidget(item, parent);
	}
	QString str;
	if (item.data().canConvert<LocalizedString>())
		str = item.data().value<LocalizedString>();
	else
		str = item.data().toString();
	QStringList alt = variantToStringList(item.property("alternatives"));
	if (!alt.isEmpty()) {
		return new ComboBox(str, alt, "validator", item, parent);
	} else if (!item.property("multiline", false)) {
		return new LineEdit(item, QString(), parent);
	} else {
		verticalPolicy = QSizePolicy::MinimumExpanding;
		return new TextEdit(item, parent);
	}
}

QWidget *getWidget(const DataItem &item, QWidget *parent, bool *twoColumn)
{
	QSizePolicy::Policy vertPolicy;
	QWidget *widget = getWidgetHelper(item, twoColumn, vertPolicy, parent);
	widget->setWindowTitle(item.title());
	if (!item.isReadOnly())
		widget->setSizePolicy(QSizePolicy::Expanding, vertPolicy);
	return widget;
}

DataItem getDataItem(QWidget *title, QWidget *data)
{
	QString titleStr;
	QLabel *label = qobject_cast<QLabel*>(title);
	if (label) {
		titleStr = label->text();
		titleStr = titleStr.left(titleStr.size()-1);
	} else {
		QComboBox *box = qobject_cast<QComboBox*>(title);
		if (box) {
			QString text = box->currentText();
			if (!text.isEmpty() && text != notSpecifiedStr)
				titleStr = text;
		}
	}
	AbstractDataWidget *dataGroup = qobject_cast<AbstractDataWidget*>(data);
	if (dataGroup) {
		DataItem item = dataGroup->item();
		item.setTitle(titleStr);
		return item;
	}
	return DataItem(data->objectName(), titleStr, QVariant());
}


} // namespace Core

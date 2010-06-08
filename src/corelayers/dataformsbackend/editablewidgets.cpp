#include "editablewidgets.h"

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

}

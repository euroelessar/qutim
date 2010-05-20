#include "editablewidgets.h"

namespace Core
{

using namespace qutim_sdk_0_3;

DataItem CheckBox::item() const
{
	bool val = isChecked();
	QVariant d;
	if (val)
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

DataItem ComboBox::item() const
{
	QString val = currentText();
	QVariant d;
	if (!val.isEmpty() && val != notSpecifiedStr)
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

DataItem DateTimeEdit::item() const
{
	QDateTime val = dateTime();
	QVariant d;
	if (val.isValid())
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

DataItem DateEdit::item() const
{
	QDate val = date();
	QVariant d;
	if (val.isValid())
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

DataItem TextEdit::item() const
{
	QString val = toPlainText();
	QVariant d;
	if (!val.isEmpty())
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

DataItem LineEdit::item() const
{
	QString val = text();
	QVariant d;
	if (!val.isEmpty())
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
}

DataItem SpinBox::item() const
{
	int val = value();
	QVariant d;
	if (val != 0)
		d = val;
	return DataItem(objectName(), LocalizedString(), d);
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

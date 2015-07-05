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
	QStringList list = data.toStringList();
	if (list.isEmpty()) {
		foreach (const LocalizedString &str, data.value<LocalizedStringList>())
			list << str;
	}
	return list;
}

QWidget *getTitle(DefaultDataForm *dataForm, const DataItem &item, const Qt::Alignment &alignment, QWidget *parent)
{
	QWidget *widget = 0;
	QStringList alt = variantToStringList(item.property("titleAlternatives"));
	if (item.isReadOnly() || alt.isEmpty()) {
		QLabel *title = new QLabel(item.title() + ":",parent);
		title->setAlignment(alignment);
		if (item.isReadOnly())
			title->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
		widget = title;
	} else {
		widget = new ComboBox(dataForm, item.title(), alt, true, item);
	}
	if (item.property("hidden", false))
		widget->setVisible(false);
	return widget;
}

static QWidget *getWidgetHelper(DefaultDataForm *dataForm, const DataItem &item, bool *twoColumn, QSizePolicy::Policy &verticalPolicy, QWidget *parent)
{
	verticalPolicy = QSizePolicy::Maximum;
	if (twoColumn)
		*twoColumn = false;
	QVariant::Type type = item.data().type();
	if (item.isReadOnly()) {
		if (item.hasSubitems()) {
			if (twoColumn)
				*twoColumn = true;
			return new DataGroup(dataForm, item, parent);
		} else {
			return new Label(dataForm, item, parent);
		}
	} else if (type == QVariant::StringList || item.data().canConvert<LocalizedStringList>()) {
		return new StringListGroup(dataForm, item, parent);
	} else if (item.isAllowedModifySubitems()) {
		if (twoColumn)
			*twoColumn = true;
		return new ModifiableGroup(dataForm, item, parent);
	} else if (item.hasSubitems()) {
		if (twoColumn)
			*twoColumn = true;
		return new DataGroup(dataForm, item, parent);
	} else if (type == QVariant::Bool) {
		if (twoColumn)
			*twoColumn = true;
		return new CheckBox(dataForm, item, parent);
	} else if (type == QVariant::Date) {
		return new DateEdit(dataForm, item, parent);
	} else if (type == QVariant::DateTime) {
		return new DateTimeEdit(dataForm, item, parent);
	} else if (type == QVariant::Int || type == QVariant::LongLong || type == QVariant::UInt) {
		return new SpinBox(dataForm, item, parent);
	} else if (type == QVariant::Double) {
		return new DoubleSpinBox(dataForm, item, parent);
	} else if (type == QVariant::Icon || type == QVariant::Pixmap || type == QVariant::Image) {
		if (item.property("alternatives").isNull())
			return new IconWidget(dataForm, item, parent);
		else
			return new IconListWidget(dataForm, item, parent);
	}
	QString str;
	if (item.data().canConvert<LocalizedString>())
		str = item.data().value<LocalizedString>();
	else
		str = item.data().toString();
	QStringList alt = variantToStringList(item.property("alternatives"));
	if (!alt.isEmpty()) {
		return new ComboBox(dataForm, str, alt, false, item, parent);
	} else if (!item.property("multiline", false)) {
		return new LineEdit(dataForm, item, QString(), parent);
	} else {
		verticalPolicy = QSizePolicy::MinimumExpanding;
		return new TextEdit(dataForm, item, parent);
	}
}

QWidget *getWidget(DefaultDataForm *dataForm, const DataItem &item, QWidget *parent, bool *twoColumn)
{
	QSizePolicy::Policy vertPolicy;
	QWidget *widget = getWidgetHelper(dataForm, item, twoColumn, vertPolicy, parent);
	widget->setWindowTitle(item.title());
	if (!item.isReadOnly())
		widget->setSizePolicy(QSizePolicy::Expanding, vertPolicy);
	if (item.property("hidden", false))
		widget->setVisible(false);
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
			if (!text.isEmpty())
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


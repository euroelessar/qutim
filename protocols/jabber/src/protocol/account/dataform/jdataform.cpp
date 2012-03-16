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

#include "jdataform_p.h"
#include <qutim/icon.h>
#include <qutim/dataforms.h>
#include <qutim/debug.h>
#include <QGridLayout>
#include <QDebug>
#include <jreen/dataform.h>
#include <jreen/jid.h>

namespace Jabber
{
using namespace qutim_sdk_0_3;

class JDataFormPrivate
{
public:
	JDataFormPrivate()
	{
	}
	~JDataFormPrivate()
	{
//		qDeleteAll(fields);
	}
	Jreen::DataForm::Ptr form;
	DataItem item;
	AbstractDataForm *widget;
//	QList<NewJDataFormElement *> fields;
};

JDataForm::JDataForm(const Jreen::DataForm::Ptr &form,
					 AbstractDataForm::StandardButtons buttons, QWidget *parent)
	: QWidget(parent), d_ptr(new JDataFormPrivate)
{
	Q_D(JDataForm);
	d->form = form; //FIXME use QSharedData
	DataItem root(form->title());
	for (int i = 0; i < form->fieldsCount(); i++) {
		Jreen::DataFormField field = form->field(i);
		debug() << "var + values" << field.var() << field.values();
		QVariant data;
		if (field.type() == Jreen::DataFormField::Boolean)
			data = field.cast<Jreen::DataFormFieldBoolean>().value();
		else if (field.type() == Jreen::DataFormField::ListMulti)
			data = field.cast<Jreen::DataFormFieldListMulti>().values();
		else if (field.type() == Jreen::DataFormField::Hidden)
			continue;
		else
			data = field.value();
		DataItem item(field.var(), field.label(), data);
		item.setReadOnly(form->type() != Jreen::DataForm::Form);
		item.setProperty("mandatory", field.isRequired());
		debug() << field.type() << field.var() << field.label() << data;
		if (field.type() == Jreen::DataFormField::TextMulti)
			item.setProperty("multiline", true);
		else if (field.type() == Jreen::DataFormField::TextPrivate)
			item.setProperty("password", true);
		Jreen::DataFormOptionContainer options = field.cast<Jreen::DataFormOptionContainer>();
		if (options.type() != Jreen::DataFormField::Invalid) {
			QStringList labels;
			QStringList ids;
			for (int i = 0; i < options.optionsCount(); i++) {
				labels << options.optionLabel(i);
				ids << options.optionValue(i);
			}
			debug() << labels << ids;
			item.setProperty("alternatives", labels);
			item.setProperty("identificators", ids);
		}
		root << item;
	}
	QGridLayout *layout = new QGridLayout();
	setLayout(layout);
	layout->addWidget(d->widget = AbstractDataForm::get(root, buttons));
}

JDataForm::~JDataForm()
{
}

qutim_sdk_0_3::AbstractDataForm *JDataForm::widget()
{
	return d_func()->widget;
}

QString optionValueByLabel(const Jreen::DataFormField &field, const QString &label)
{
	Jreen::DataFormOptionContainer options = field.cast<Jreen::DataFormOptionContainer>();
	for (int i = 0; i < options.optionsCount(); i++) {
		if (options.optionLabel(i) == label)
			return options.optionValue(i);
	}
	return QString();
}

Jreen::DataForm::Ptr JDataForm::getDataForm()
{
	Q_D(JDataForm);
	d->form->setType(Jreen::DataForm::Submit);
	DataItem item = d->widget->item();
	for (int i = 0; i < d->form->fieldsCount(); i++) {
		Jreen::DataFormField field = d->form->field(i);
		DataItem currentItem = item.subitem(field.var());
		QVariant data = currentItem.data();
		
		if (field.type() == Jreen::DataFormField::Boolean) {
			field.cast<Jreen::DataFormFieldBoolean>().setValue(data.toBool());
		} else if (field.type() == Jreen::DataFormField::ListSingle) {
			field.setValue(currentItem.property("identificator").toString());
		} else if (field.type() == Jreen::DataFormField::ListMulti) {
			QStringList values = data.toStringList();
			for (int i = 0; i < values.size(); i++)
				values[i] = optionValueByLabel(field, values.at(i));
			field.setValues(values);
		} else if (field.type() == Jreen::DataFormField::JidMulti) {
			field.setValues(data.toStringList());
		} else {
			field.setValue(data.toString());
		}
	}
	return d->form;
}
}


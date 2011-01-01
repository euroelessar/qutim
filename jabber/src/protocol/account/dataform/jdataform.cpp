/****************************************************************************
 *  jdataform.cpp
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                     by Denis Daschenko <daschenko@gmail.com>
 *                     by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include "jdataform_p.h"
#include <qutim/icon.h>
#include <qutim/dataforms.h>
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
	jreen::DataForm::Ptr form;
	DataItem item;
	AbstractDataForm *widget;
//	QList<NewJDataFormElement *> fields;
};

JDataForm::JDataForm(const jreen::DataForm::Ptr &form,
					 AbstractDataForm::StandardButtons buttons, QWidget *parent)
	: QWidget(parent), d_ptr(new JDataFormPrivate)
{
	Q_D(JDataForm);
	d->form = form; //FIXME use QSharedData
	DataItem root(form->title());
	for (int i = 0; i < form->fieldsCount(); i++) {
		jreen::DataFormField field = form->field(i);
		QVariant data;
		if (field.type() == jreen::DataFormField::Boolean)
			data = field.cast<jreen::DataFormFieldBoolean>().value();
		else if (field.type() == jreen::DataFormField::ListMulti)
			data = field.cast<jreen::DataFormFieldListMulti>().values();
		else if (field.type() == jreen::DataFormField::Hidden)
			continue;
		else
			data = field.value();
		DataItem item(field.var(), field.label(), data);
		item.setReadOnly(form->type() != jreen::DataForm::Form);
		item.setDataChangedHandler(this, SLOT(onItemChanged(QString,QVariant)));
		if (field.isRequired())
			item.setProperty("mandatory", true);
		qDebug() << field.type() << field.var() << field.label() << data;
		if (field.type() == jreen::DataFormField::TextMulti)
			item.setProperty("multiline", true);
		else if (field.type() == jreen::DataFormField::TextPrivate)
			item.setProperty("password", true);
		jreen::DataFormOptionContainer options = field.cast<jreen::DataFormOptionContainer>();
		if (options.type() != jreen::DataFormField::Invalid) {
			QStringList labels;
			for (int i = 0; i < options.optionsCount(); i++)
				labels << options.optionLabel(i);
			qDebug() << labels;
			item.setProperty("alternatives", labels);
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

jreen::DataForm::Ptr JDataForm::getDataForm()
{
	Q_D(JDataForm);
	d->form->setType(jreen::DataForm::Submit);
	return d->form;
}

QString optionValueByLabel(const jreen::DataFormField &field, const QString &label)
{
	jreen::DataFormOptionContainer options = field.cast<jreen::DataFormOptionContainer>();
	for (int i = 0; i < options.optionsCount(); i++) {
		if (options.optionLabel(i) == label)
			return options.optionValue(i);
	}
	return QString();
}

void JDataForm::onItemChanged(const QString &name, const QVariant &data)
{
	Q_D(JDataForm);
	jreen::DataFormField field = d->form->field(name);
	
	if (field.type() == jreen::DataFormField::Boolean) {
		field.cast<jreen::DataFormFieldBoolean>().setValue(data.toBool());
	} else if (field.type() == jreen::DataFormField::ListSingle) {
		QString value = optionValueByLabel(field, data.toString());
		field.setValue(value);
	} else if (field.type() == jreen::DataFormField::ListMulti) {
		QStringList values = data.toStringList();
		for (int i = 0; i < values.size(); i++)
			values[i] = optionValueByLabel(field, values.at(i));
		field.setValues(values);
	} else if (field.type() == jreen::DataFormField::JidMulti) {
		field.setValues(data.toStringList());
	} else {
		field.setValue(data.toString());
	}
}

}

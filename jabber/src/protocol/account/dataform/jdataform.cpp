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
#include <QGridLayout>
#include <QDebug>
#include <jreen/dataform.h>
#include <jreen/jid.h>

namespace Jabber
{

void dataform_add_widget_default(QGridLayout *layout, QObject *obj, const QString &label, int row, int column)
{
	QWidget * const widget = static_cast<QWidget *>(obj);
	layout->addWidget(new QLabel(label, widget->parentWidget()), row, column, Qt::AlignLeft | Qt::AlignTop);
	layout->addWidget(widget, row, column + 1);
}

//		TypeBoolean,              /**< The field enables an entity to gather or provide an either-or
//									* choice  between two options. The default value is "false". */
QObject *dataform_new_instance_boolean(QWidget *parent)
{ return new QCheckBox(parent); }
void dataform_import_value_boolean(QObject *obj, jreen::DataFormField *field)
{ static_cast<QCheckBox *>(obj)->setChecked(field->value() == "1"); }
void dataform_export_value_boolean(QObject *obj, jreen::DataFormField *field)
{ field->setValues(QStringList(static_cast<QCheckBox *>(obj)->isChecked() ? "1" : "0")); }
void dataform_add_widget_boolean(QGridLayout *layout, QObject *obj, const QString &label, int row, int column)
{
	QCheckBox * const box = static_cast<QCheckBox *>(obj);
	box->setText(label);
	layout->addWidget(box, row, column, 1, 2);
}

//		TypeFixed,                /**< The field is intended for data description (e.g.,
//									* human-readable text such as "section" headers) rather than data
//									* gathering or provision. The &lt;value/&gt; child SHOULD NOT contain
//									* newlines (the \\n and \\r characters); instead an application SHOULD
//									* generate multiple fixed fields, each with one &lt;value/&gt; child. */
QObject *dataform_new_instance_fixed(QWidget *parent)
{
	QLabel * const label = new QLabel(parent);
	label->setWordWrap(true);
	return label;
}
void dataform_import_value_fixed(QObject *obj, jreen::DataFormField *field)
{ static_cast<QLabel *>(obj)->setText(field->value()); }
void dataform_add_widget_fixed(QGridLayout *layout, QObject *obj, const QString &label, int row, int column)
{ layout->addWidget(static_cast<QLabel *>(obj), row, column, 1, 2); }
//		TypeHidden,               /**< The field is not shown to the entity providing information, but
//									* instead is returned with the form. */

//		TypeJidMulti,             /**< The field enables an entity to gather or provide multiple Jabber
//									* IDs.*/
JDataFormJidMulti::JDataFormJidMulti(QWidget *parent) : QListWidget(parent)
{
	setItemDelegate(new JDataFormJidMultiItemDelegate(this));
	layout = new QGridLayout(parent);
	layout->setMargin(0);
	jidEdit = new QLineEdit(parent);
	jidEdit->setValidator(new JJidValidator(QString(), parent));
	addButton = new QPushButton(parent);
	addButton->setIcon(qutim_sdk_0_3::Icon("list-add-user"));
	addButton->setDisabled(true);
	removeButton = new QPushButton(parent);
	removeButton->setIcon(qutim_sdk_0_3::Icon("list-remove-user"));
	removeButton->setDisabled(true);
	layout->addWidget(jidEdit, 0, 0);
	layout->addWidget(addButton, 0, 1, Qt::AlignTop);
	layout->addWidget(removeButton, 1, 1, Qt::AlignTop);
	layout->addWidget(this, 1, 0);
	connect(jidEdit, SIGNAL(textChanged(QString)), this, SLOT(jidEditTextChanged(QString)));
	connect(addButton, SIGNAL(clicked()), this, SLOT(addButtonClicked()));
	connect(removeButton, SIGNAL(clicked()), this, SLOT(removeButtonClicked()));
}

void JDataFormJidMulti::selectionChanged(const QItemSelection &, const QItemSelection &)
{
	removeButton->setEnabled(!selectedItems().isEmpty());
}

void JDataFormJidMulti::jidEditTextChanged(const QString &jid)
{
	addButton->setEnabled(!jid.isEmpty());
}

void JDataFormJidMulti::addButtonClicked()
{
	QListWidgetItem *item = new QListWidgetItem(jidEdit->text(), this);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
	jidEdit->clear();
}

void JDataFormJidMulti::removeButtonClicked()
{
	qDeleteAll(selectedItems());
}

QObject *dataform_new_instance_jid_multi(QWidget *parent)
{ return new JDataFormJidMulti(parent); }
void dataform_import_value_jid_multi(QObject *obj, jreen::DataFormField *field)
{
	JDataFormJidMulti * const list = static_cast<JDataFormJidMulti *>(obj);
	foreach(const QString &str, field->values()) {
		QString jid = jreen::JID(str).bare();
		QListWidgetItem *item = new QListWidgetItem(jid, list);
		item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
	}
}
void dataform_export_value_jid_multi(QObject *obj, jreen::DataFormField *field)
{
	JDataFormJidMulti * const list = static_cast<JDataFormJidMulti *>(obj);
	QStringList lines;
	for (int i = 0, size = list->count(); i < size; i++)
		lines.push_back(list->item(i)->text());
	field->setValues(lines);
}
void dataform_add_widget_jid_multi(QGridLayout *layout, QObject *obj, const QString &label, int row, int column)
{
	JDataFormJidMulti * const list = static_cast<JDataFormJidMulti *>(obj);
	layout->addWidget(new QLabel(label, list->parentWidget()), row, column, Qt::AlignLeft | Qt::AlignTop);
	layout->addLayout(list->localLayout(), row, column + 1);
}
//		TypeJidSingle,            /**< The field enables an entity to gather or provide a single Jabber
//									* ID.*/
QObject *dataform_new_instance_jid_single(QWidget *parent)
{
	QLineEdit * const lineEdit = new QLineEdit(parent);
	lineEdit->setValidator(new JJidValidator(QString(), lineEdit));
	return lineEdit;
}
//		TypeListMulti,            /**< The field enables an entity to gather or provide one or more options
//									* from among many. */
QObject *dataform_new_instance_list_multi(QWidget *parent)
{
	return new QListWidget(parent);
}
void dataform_import_value_list_multi(QObject *obj, jreen::DataFormField *field)
{
	QListWidget * const list = static_cast<QListWidget *>(obj);
	qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!" << list->itemDelegate();
	list->setMinimumHeight(0);

	const jreen::DataFormOptionList &options = field->options();
	jreen::DataFormOptionList::const_iterator it = options.begin();
	QStringList values = field->values();
	for (; it != options.end(); ++it) {
		//QListWidgetItem *item = new QListWidgetItem(, list);
		//item->setData(Qt::UserRole, (*it).second);
		//item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		//item->setCheckState(values.contains((*it).second) ? Qt::Unchecked : Qt::Checked);
	}
}
void dataform_export_value_list_multi(QObject *obj, jreen::DataFormField *field)
{
	QListWidget * const list = static_cast<QListWidget *>(obj);
	QStringList lines;
	for (int i = 0, size = list->count(); i < size; i++)
		lines.append(list->item(i)->data(Qt::UserRole).toString());
	field->setValues(lines);
}
//		TypeListSingle,           /**< The field enables an entity to gather or provide one option from
//									* among many. */
QObject *dataform_new_instance_list_single(QWidget *parent)
{ return new QComboBox(parent); }
void dataform_import_value_list_single(QObject *obj, jreen::DataFormField *field)
{
	//QComboBox * const box = static_cast<QComboBox *>(obj);
	//const StringMultiMap &options = field->options();
	//StringMultiMap::const_iterator it = options.begin();
	//for (; it != options.end(); ++it) {
	//	box->addItem(QString::fromStdString((*it).first),
	//				 QString::fromStdString((*it).second));
	//	if (field->value() == (*it).second)
	//		box->setCurrentIndex(box->count() - 1);
	//}
}
void dataform_export_value_list_single(QObject *obj, jreen::DataFormField *field)
{
	QComboBox * const box = static_cast<QComboBox *>(obj);
	field->setValues(QStringList(box->itemData(box->currentIndex()).toString()));
}
//		TypeTextMulti,            /**< The field enables an entity to gather or provide multiple lines of
//									* text. */
QObject *dataform_new_instance_text_multi(QWidget *parent)
{ return new QTextEdit(parent); }
void dataform_import_value_text_multi(QObject *obj, jreen::DataFormField *field)
{
	QTextEdit * const textEdit = static_cast<QTextEdit *>(obj);
	QString text;
	foreach (const QString &str, field->values()) {
		text += str;
		text += QLatin1Char('\n');
	}
	text.chop(1);
	textEdit->setPlainText(text);
}
void dataform_export_value_text_multi(QObject *obj, jreen::DataFormField *field)
{
	QTextEdit * const textEdit = static_cast<QTextEdit *>(obj);
	QStringList lines;
	foreach (const QString &str, textEdit->toPlainText().split('\n'))
		lines.append(str);
	field->setValues(lines);
}
//		TypeTextPrivate,          /**< The field enables an entity to gather or provide a single line or
//									* word of text, which shall be obscured in an interface
//									* (e.g., *****). */
QObject *dataform_new_instance_text_private(QWidget *parent)
{
	QLineEdit * const lineEdit = new QLineEdit(parent);
	lineEdit->setEchoMode(QLineEdit::Password);
	return lineEdit;
}
//		TypeTextSingle,           /**< The field enables an entity to gather or provide a single line or
//									* word of text, which may be shown in an interface. This field type is
//									* the default and MUST be assumed if an entity receives a field type it
//									* does not understand.*/
QObject *dataform_new_instance_text_single(QWidget *parent)
{ return new QLineEdit(parent); }
void dataform_import_value_text_single(QObject *obj, jreen::DataFormField *field)
{ static_cast<QLineEdit *>(obj)->setText(field->value()); }
void dataform_export_value_text_single(QObject *obj, jreen::DataFormField *field)
{ field->setValues(QStringList(static_cast<QLineEdit *>(obj)->text())); }
//		TypeNone,                 /**< The field is child of either a &lt;reported&gt; or &lt;item&gt;
//									* element or has no type attribute. */
//		TypeInvalid               /**< The field is invalid. Only possible if the field was created from
//									* a Tag not correctly describing a Data Form Field. */


static struct NewJDataFormElement
{
	QObject *obj;
	jreen::DataFormField *field;
	QObject *(*new_instance)(QWidget *parent);
	void (*import_value)(QObject *obj, jreen::DataFormField *field);
	void (*export_value)(QObject *obj, jreen::DataFormField *field);
	void (*add_widget)(QGridLayout *layout, QObject *obj, const QString &label, int row, int column);
} dataform_elements[]  =
{
	//		TypeBoolean
	{ 0, 0, &dataform_new_instance_boolean, &dataform_import_value_boolean,
	  &dataform_export_value_boolean, &dataform_add_widget_boolean },
	//		TypeFixed
	{ 0, 0, &dataform_new_instance_fixed, &dataform_import_value_fixed,
	  0, &dataform_add_widget_fixed },
	//		TypeHidden
	{ 0, 0, 0, 0,
	  0, 0 },
	//		TypeJidMulti
	{ 0, 0, &dataform_new_instance_jid_multi, &dataform_import_value_jid_multi,
	  &dataform_export_value_jid_multi, &dataform_add_widget_jid_multi },
	//		TypeJidSingle
	{ 0, 0, &dataform_new_instance_jid_single, &dataform_import_value_text_single,
	  &dataform_export_value_text_single, &dataform_add_widget_default },
	//		TypeListMulti
	{ 0, 0, &dataform_new_instance_list_multi, &dataform_import_value_list_multi,
	  &dataform_export_value_list_multi, &dataform_add_widget_default },
	//		TypeListSingle
	{ 0, 0, &dataform_new_instance_list_single, &dataform_import_value_list_single,
	  &dataform_export_value_list_single, &dataform_add_widget_default },
	//		TypeTextMulti
	{ 0, 0, &dataform_new_instance_text_multi, &dataform_import_value_text_multi,
	  &dataform_export_value_text_multi, &dataform_add_widget_default },
	//		TypeTextPrivate
	{ 0, 0, &dataform_new_instance_text_private, &dataform_import_value_text_single,
	  &dataform_export_value_text_single, &dataform_add_widget_default},
	//		TypeTextSingle
	{ 0, 0, &dataform_new_instance_text_single, &dataform_import_value_text_single,
	  &dataform_export_value_text_single, &dataform_add_widget_default}
};

class JDataFormPrivate
{
public:
	JDataFormPrivate()
	{
		form = 0;
	}
	~JDataFormPrivate()
	{
		delete form;
		qDeleteAll(fields);
	}
	jreen::DataForm *form;
	QList<NewJDataFormElement *> fields;
};

JDataForm::JDataForm(const QSharedPointer<jreen::DataForm> &form, QWidget *parent)
	: QWidget(parent), d_ptr(new JDataFormPrivate)
{
	Q_D(JDataForm);
	d->form = form.data(); //FIXME use QSharedData
	QGridLayout *layout = new QGridLayout();
	setLayout(layout);
	jreen::DataFormFieldList fields = form->fields();
	int fieldCount = fields.count();
	for (int num = 0; num < fieldCount; num++) {
		jreen::DataFormField *field = fields.at(num).data();
		if (field->type() >= static_cast<jreen::DataFormField::Type>((sizeof(dataform_elements)/sizeof(NewJDataFormElement)))
				|| !dataform_elements[field->type()].new_instance)
			continue;
		NewJDataFormElement *elem = new NewJDataFormElement(dataform_elements[field->type()]);
		elem->obj = (*elem->new_instance)(this);
		elem->obj->setObjectName(field->var());
		elem->field = field;
		if (elem->import_value)
			(*elem->import_value)(elem->obj, field);
		d->fields.append(elem);

		if (!elem->add_widget)
			continue;

		if(QWidget *widget = qobject_cast<QWidget *>(elem->obj))
			widget->setToolTip(field->desc());

		int row = layout->rowCount();
		int column = 0;
		(*elem->add_widget)(layout, elem->obj, field->desc(), row, column);
	}
}

JDataForm::~JDataForm()
{
}

jreen::DataForm *JDataForm::getDataForm()
{
	//Q_D(JDataForm);
	//jreen::DataForm *form = new DataForm(*d->form);
	//form->setType(TypeSubmit);
	//foreach (NewJDataFormElement *dataField, d->fields) {
	//	if (dataField->export_value) {
	//		std::string name = dataField->obj->objectName();
	//		(*dataField->export_value)(dataField->obj, form->field(name));
	//	}
	//}
	//return form;
}
}

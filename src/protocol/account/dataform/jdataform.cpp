#include "jdataform.h"
#include <QFormLayout>

namespace Jabber
{

	void dataform_add_widget_default(QGridLayout *layout, QObject *obj, const QString &label, int row, int column)
	{
		QWidget * const widget = static_cast<QWidget *>(obj);
		layout->addWidget(new QLabel(label, widget->parentWidget()), row, column);
		layout->addWidget(widget, row, column + 1);
	}

//		TypeBoolean,              /**< The field enables an entity to gather or provide an either-or
//									* choice  between two options. The default value is "false". */
	QObject *dataform_new_instance_boolean(QWidget *parent)
	{ return new QCheckBox(parent); }
	void dataform_import_value_boolean(QObject *obj, gloox::DataFormField *field)
	{ static_cast<QCheckBox *>(obj)->setChecked(field->value() == "1"); }
	void dataform_export_value_boolean(QObject *obj, gloox::DataFormField *field)
	{ field->setValue(static_cast<QCheckBox *>(obj)->isChecked() ? "1" : "0"); }
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
	void dataform_import_value_fixed(QObject *obj, gloox::DataFormField *field)
	{ static_cast<QLabel *>(obj)->setText(QString::fromStdString(field->value())); }
//	void dataform_export_value_fixed(QObject *obj, gloox::DataFormField *field)
//	{ field->setValue(static_cast<QLabel *>(obj)->text().toStdString()); }
	void dataform_add_widget_fixed(QGridLayout *layout, QObject *obj, const QString &label, int row, int column)
	{ layout->addWidget(static_cast<QLabel *>(obj), row, column, 1, 2); }
//		TypeHidden,               /**< The field is not shown to the entity providing information, but
//									* instead is returned with the form. */
	QObject *dataform_new_instance_hidden(QWidget *parent)
	{ return new QObject(parent); }
	void dataform_import_value_hidden(QObject *obj, gloox::DataFormField *field)
	{ obj->setProperty("value", QString::fromStdString(field->value())); }
	void dataform_export_value_hidden(QObject *obj, gloox::DataFormField *field)
	{ field->setValue(obj->property("value").toString().toStdString()); }
	void dataform_add_widget_hidden(QGridLayout *layout, QObject *obj, const QString &label, int row, int column)
	{}
//+		TypeJidMulti,             /**< The field enables an entity to gather or provide multiple Jabber
//									* IDs.*/
//		TypeJidSingle,            /**< The field enables an entity to gather or provide a single Jabber
//									* ID.*/
//+		TypeListMulti,            /**< The field enables an entity to gather or provide one or more options
//									* from among many. */
//		TypeListSingle,           /**< The field enables an entity to gather or provide one option from
//									* among many. */
	QObject *dataform_new_instance_list_single(QWidget *parent)
	{ return new QComboBox(parent); }
	void dataform_import_value_list_single(QObject *obj, gloox::DataFormField *field)
	{
		QComboBox * const box = static_cast<QComboBox *>(obj);
		const StringMultiMap &options = field->options();
		StringMultiMap::const_iterator it = options.begin();
		for (; it != options.end(); ++it) {
			box->addItem(QString::fromStdString((*it).first),
						 QString::fromStdString((*it).second));
		}
	}
	void dataform_export_value_list_single(QObject *obj, gloox::DataFormField *field)
	{
		QComboBox * const box = static_cast<QComboBox *>(obj);
		field->setValue(box->itemData(box->currentIndex()).toString().toStdString());
	}
//+		TypeTextMulti,            /**< The field enables an entity to gather or provide multiple lines of
//									* text. */
	QObject *dataform_new_instance_text_multi(QWidget *parent)
	{ return new QTextEdit(parent); }
	void dataform_import_value_text_multi(QObject *obj, gloox::DataFormField *field)
	{
		QTextEdit * const textEdit = static_cast<QTextEdit *>(obj);
		QString text;
		foreach (const std::string &str, field->values()) {
			text += QString::fromStdString(str);
			text += QLatin1Char('\n');
		}
		text.chop(1);
		textEdit->setPlainText(text);
	}
	void dataform_export_value_text_multi(QObject *obj, gloox::DataFormField *field)
	{
		QTextEdit * const textEdit = static_cast<QTextEdit *>(obj);
		StringList lines;
		foreach (const QString &str, textEdit->toPlainText().split('\n'))
			lines.push_back(str.toStdString());
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
	void dataform_import_value_text_single(QObject *obj, gloox::DataFormField *field)
	{ static_cast<QLineEdit *>(obj)->setText(QString::fromStdString(field->value())); }
	void dataform_export_value_text_single(QObject *obj, gloox::DataFormField *field)
	{ field->setValue(static_cast<QLineEdit *>(obj)->text().toStdString()); }
//		TypeNone,                 /**< The field is child of either a &lt;reported&gt; or &lt;item&gt;
//									* element or has no type attribute. */
//		TypeInvalid               /**< The field is invalid. Only possible if the field was created from
//									* a Tag not correctly describing a Data Form Field. */


	static struct NewJDataFormElement
	{
		QObject *obj;
		gloox::DataFormField *field;
		QObject *(*new_instance)(QWidget *parent);
		void (*import_value)(QObject *obj, gloox::DataFormField *field);
		void (*export_value)(QObject *obj, gloox::DataFormField *field);
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
	/*	{ 0, 0, &dataform_new_instance_hidden, &dataform_import_value_hidden,
		  &dataform_export_value_hidden, &dataform_add_widget_hidden },
	*/
//		TypeJidMulti
		{ 0, 0, 0, 0,
		  0, &dataform_add_widget_default },
//		TypeJidSingle
		{ 0, 0, 0, 0,
		  0, &dataform_add_widget_default },
//		TypeListMulti
		{ 0, 0, 0, 0,
		  0, &dataform_add_widget_default },
//		TypeListSingle
		{ 0, 0, dataform_new_instance_list_single, dataform_import_value_list_single,
		  dataform_export_value_list_single, &dataform_add_widget_default },
//		TypeTextMulti
		{ 0, 0, dataform_new_instance_text_multi, dataform_import_value_text_multi,
		  dataform_export_value_text_multi, &dataform_add_widget_default },
//		TypeTextPrivate
		{ 0, 0, &dataform_new_instance_text_private, &dataform_import_value_text_single,
		  &dataform_export_value_text_single, &dataform_add_widget_default},
//		TypeTextSingle
		{ 0, 0, &dataform_new_instance_text_single, &dataform_import_value_text_single,
		  &dataform_export_value_text_single, &dataform_add_widget_default}
	};

	struct JDataFormPrivate
	{
		JDataFormPrivate()
		{
			form = 0;
		}
		~JDataFormPrivate()
		{
			delete form;
			qDeleteAll(fields);
		}
		DataForm *form;
		QList<NewJDataFormElement *> fields;
	};

	JDataForm::JDataForm(const DataForm *form, bool twocolumn, QWidget *parent)
			: QWidget(parent), p(new JDataFormPrivate)
	{
		p->form = new DataForm(*form);
		p->form->setType(TypeSubmit);
		QGridLayout *layout = new QGridLayout();
		setLayout(layout);
		QList<DataFormField*> fields = QList<DataFormField*>::fromStdList(form->fields());
		bool skip = false;
		int fieldCount = fields.count();
		for (int num = 0; num < fieldCount; num++) {
			DataFormField *field = fields[num];
			if (field->type() >= (sizeof(dataform_elements)/sizeof(NewJDataFormElement))
				|| !dataform_elements[field->type()].new_instance)
				continue;
			NewJDataFormElement *elem = new NewJDataFormElement(dataform_elements[field->type()]);
			elem->obj = (*elem->new_instance)(this);
			elem->obj->setObjectName(QString::fromStdString(field->name()));
			elem->field = field;
			if (elem->import_value)
				(*elem->import_value)(elem->obj, field);
			p->fields.append(elem);

			if (field->type() == DataFormField::TypeHidden)
				continue;

			if (!twocolumn ||
					(num < fieldCount-1 && fields[num+1]->type() == DataFormField::TypeFixed
					|| num && fields[num-1]->type() == DataFormField::TypeFixed
					|| fields[num]->type() == DataFormField::TypeFixed)) {
				skip = false;
			}
			int row = layout->rowCount(), column = 0;
			if (field->type() != DataFormField::TypeFixed && skip) {
				row--;
				column = 2;
			}
			if (elem->add_widget)
				(*elem->add_widget)(layout, elem->obj, QString::fromStdString(field->label()), row, column);
			skip = !skip;
		}
	}

	JDataForm::~JDataForm()
	{
	}

	DataForm *JDataForm::getDataForm()
	{
		foreach (NewJDataFormElement *dataField, p->fields) {
			if (dataField->export_value) {
				std::string name = dataField->obj->objectName().toStdString();
				(*dataField->export_value)(dataField->obj, p->form->field(name));
			}
		}
		return new DataForm(*p->form);
	}
}

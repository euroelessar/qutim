#include "jdataform.h"
#include <QFormLayout>

namespace Jabber
{

	void dataform_add_widget_default(QGridLayout *layout, QObject *obj, const QString &label, int row, int column)
	{
		QWidget * const widget = static_cast<QWidget *>(obj);
		layout->addWidget(new QLabel(label, widget->parentWidget()), row, column + 1);
		layout->addWidget(widget, row, column);
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
	{ return new QLabel(parent); }
	void dataform_import_value_fixed(QObject *obj, gloox::DataFormField *field)
	{ static_cast<QLabel *>(obj)->setText(QString::fromStdString(field->value())); }
	void dataform_export_value_fixed(QObject *obj, gloox::DataFormField *field)
	{ field->setValue(static_cast<QLabel *>(obj)->text().toStdString()); }
	void dataform_add_widget_fixed(QGridLayout *layout, QObject *obj, const QString &label, int row, int column)
	{
		QLabel * const widget = static_cast<QLabel *>(obj);
		widget->setText(label);
		layout->addWidget(widget, row, column, 1, 2);
	}
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
//		TypeJidMulti,             /**< The field enables an entity to gather or provide multiple Jabber
//									* IDs.*/
//		TypeJidSingle,            /**< The field enables an entity to gather or provide a single Jabber
//									* ID.*/
//		TypeListMulti,            /**< The field enables an entity to gather or provide one or more options
//									* from among many. */
//		TypeListSingle,           /**< The field enables an entity to gather or provide one option from
//									* among many. */
//		TypeTextMulti,            /**< The field enables an entity to gather or provide multiple lines of
//									* text. */
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
		QObject *(*new_instance)(QWidget *parent);
		void (*import_value)(QObject *obj, gloox::DataFormField *field);
		void (*export_value)(QObject *obj, gloox::DataFormField *field);
		void (*add_widget)(QGridLayout *layout, QObject *obj, const QString &label, int row, int column);
	} dataform_elements[]  =
	{
		{ &dataform_new_instance_boolean, &dataform_import_value_boolean, &dataform_export_value_boolean, &dataform_add_widget_boolean },
		{ &dataform_new_instance_fixed, &dataform_import_value_fixed, &dataform_export_value_fixed, &dataform_add_widget_fixed },
		{ &dataform_new_instance_hidden, &dataform_import_value_hidden, &dataform_export_value_hidden, &dataform_add_widget_hidden },
		{ 0, 0, 0, &dataform_add_widget_default },
		{ 0, 0, 0, &dataform_add_widget_default },
		{ 0, 0, 0, &dataform_add_widget_default },
		{ 0, 0, 0, &dataform_add_widget_default },
		{ 0, 0, 0, &dataform_add_widget_default },
		{ &dataform_new_instance_text_private, &dataform_import_value_text_single, &dataform_export_value_text_single, &dataform_add_widget_default},
		{ &dataform_new_instance_text_single, &dataform_import_value_text_single, &dataform_export_value_text_single, &dataform_add_widget_default}
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
		}
		DataForm *form;
		QList<JDataFormElement *> fields;
	};

	JDataForm::JDataForm(const DataForm *form, bool twocolumn, QWidget *parent)
			: QWidget(parent), p(new JDataFormPrivate)
	{
		p->form = new DataForm(*form);
		p->form->setType(TypeSubmit);
		QList<DataFormField*> fields = QList<DataFormField*>::fromStdList(form->fields());
		QGridLayout *layout = new QGridLayout();
		this->setLayout(layout);
		bool skip = false;
		int fieldCount = fields.count();
		for (int num = 0; num < fieldCount; num++) {
			DataFormField *field = fields[num];
			QLabel *label = new QLabel();
			JDataFormElement *df = 0;
			switch (field->type()) {
			case DataFormField::TypeTextPrivate:
				df = new JDFSingleText();
				qobject_cast<JDFSingleText *>(df->instance())->setEchoMode(QLineEdit::Password);
				break;
			case DataFormField::TypeTextSingle:
				df = new JDFSingleText();
				//widget = line_edits.last();
				break;
			case DataFormField::TypeTextMulti:
				df = new JDFMultiText();
				//widget = text_edits.last();
				break;
			case DataFormField::TypeBoolean:
				df = new JDFBoolean();
				qobject_cast<JDFBoolean *>(df->instance())->setText(QString::fromStdString(field->label()));
				break;
			case DataFormField::TypeHidden:
				df = new JDFHidden();
				break;
			case DataFormField::TypeListSingle:
				df = new JDFSingleList();
				{
					StringMultiMap options = field->options();
					StringMultiMap::const_iterator it2 = options.begin();
					for( ; it2 != options.end(); ++it2 )
						qobject_cast<JDFSingleList *>(df->instance())->addItem(
								QString::fromStdString((*it2).first),
								QString::fromStdString((*it2).second));
				}
				break;
			case DataFormField::TypeListMulti:
				df = new JDFMultiList();
				break;
			case DataFormField::TypeJidSingle:
				df = new JDFSingleJID();
				break;
			case DataFormField::TypeJidMulti:
				df = new JDFMultiJID();
				break;
			}
			if (field->type() != DataFormField::TypeFixed) {
				df->dfSetName(QString::fromStdString(field->name()));
				df->dfSetValue(QString::fromStdString(field->value()));
				p->fields.append(df);
			} else if (field->type() != DataFormField::TypeBoolean) {
				label->setText(QString::fromStdString(field->label()));
				label->setWordWrap(true);
			}
			if (field->type() == DataFormField::TypeHidden)
				continue;

			if (!twocolumn ||
					(num < fieldCount-1 && fields[num+1]->type() == DataFormField::TypeFixed
					|| num && fields[num-1]->type() == DataFormField::TypeFixed
					|| fields[num]->type() == DataFormField::TypeFixed)) {
				skip = false;
			}
			if (df) {
				QWidget *widget = qobject_cast<QWidget *>(df->instance());
				if (skip) {
					if (label->text().isEmpty()) {
						layout->addWidget(widget, layout->rowCount()-1, 2, 1, 2);
					} else {
						layout->addWidget(label, layout->rowCount()-1, 2);
						layout->addWidget(widget, layout->rowCount()-1, 3);
					}
				} else {
					if (label->text().isEmpty()) {
						layout->addWidget(widget, layout->rowCount(), 0, 1, 2);
					} else {
						layout->addWidget(label, layout->rowCount(), 0);
						layout->addWidget(widget, layout->rowCount()-1, 1);
					}
				}
			} else {
				layout->addWidget(label, layout->rowCount(), 0, 1, 2);
			}
			skip = !skip;
		}
	}

	JDataForm::~JDataForm()
	{
	}

	DataForm *JDataForm::getDataForm()
	{
		foreach (JDataFormElement *dataField, p->fields)
		{
			std::string name = dataField->dfName().toStdString();
			std::string value = dataField->dfValue().toStdString();
			p->form->field(name)->setValue(value);
		}
		return new DataForm(*p->form);
	}
}

#include "jdataform_p.h"
#include <qutim/icon.h>
#include <QGridLayout>
#include <QDebug>

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
		jidEdit->setValidator(new JJidValidator(gloox::EmptyString, parent));
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
	void dataform_import_value_jid_multi(QObject *obj, gloox::DataFormField *field)
	{
		JDataFormJidMulti * const list = static_cast<JDataFormJidMulti *>(obj);
		foreach(const std::string &str, field->values()) {
			QString jid = QString::fromStdString(JID(str).bare());
			QListWidgetItem *item = new QListWidgetItem(jid, list);
			item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
		}
	}
	void dataform_export_value_jid_multi(QObject *obj, gloox::DataFormField *field)
	{
		JDataFormJidMulti * const list = static_cast<JDataFormJidMulti *>(obj);
		StringList lines;
		for (int i = 0, size = list->count(); i < size; i++)
			lines.push_back(list->item(i)->text().toStdString());
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
		lineEdit->setValidator(new JJidValidator(EmptyString, lineEdit));
		return lineEdit;
	}
//		TypeListMulti,            /**< The field enables an entity to gather or provide one or more options
//									* from among many. */
	QObject *dataform_new_instance_list_multi(QWidget *parent)
	{
		return new QListWidget(parent);
	}
	void dataform_import_value_list_multi(QObject *obj, gloox::DataFormField *field)
	{
		QListWidget * const list = static_cast<QListWidget *>(obj);
		qDebug() << "!!!!!!!!!!!!!!!!!!!!!!!" << list->itemDelegate();
		list->setMinimumHeight(0);

		const StringMultiMap &options = field->options();
		StringMultiMap::const_iterator it = options.begin();
		for (; it != options.end(); ++it) {
			QListWidgetItem *item = new QListWidgetItem(QString::fromStdString((*it).first), list);
			item->setData(Qt::UserRole, QString::fromStdString((*it).second));
			item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
			item->setCheckState(Qt::Unchecked);
		}
	}
	void dataform_export_value_list_multi(QObject *obj, gloox::DataFormField *field)
	{
		QListWidget * const list = static_cast<QListWidget *>(obj);
		StringList lines;
		for (int i = 0, size = list->count(); i < size; i++)
			lines.push_back(list->item(i)->data(Qt::UserRole).toString().toStdString());
		field->setValues(lines);
	}
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
//		TypeTextMulti,            /**< The field enables an entity to gather or provide multiple lines of
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

			if (!elem->add_widget)
				continue;

			if(QWidget *widget = qobject_cast<QWidget *>(elem->obj))
				widget->setToolTip(QString::fromStdString(field->description()));

			if (!twocolumn
				|| (num < fieldCount-1 && fields[num+1]->type() == DataFormField::TypeFixed)
				|| (num && fields[num-1]->type() == DataFormField::TypeFixed)
				|| (fields[num]->type() == DataFormField::TypeFixed)) {
				skip = false;
			}
			int row = layout->rowCount();
			int column = 0;
			if (skip && field->type() != DataFormField::TypeFixed) {
				row--;
				column = 2;
			}
			(*elem->add_widget)(layout, elem->obj, QString::fromStdString(field->label()), row, column);
			skip = !skip;
		}
	}

	JDataForm::~JDataForm()
	{
	}

	DataForm *JDataForm::getDataForm()
	{
		DataForm *form = new DataForm(*p->form);
		form->setType(TypeSubmit);
		foreach (NewJDataFormElement *dataField, p->fields) {
			if (dataField->export_value) {
				std::string name = dataField->obj->objectName().toStdString();
				(*dataField->export_value)(dataField->obj, form->field(name));
			}
		}
		return form;
	}
}

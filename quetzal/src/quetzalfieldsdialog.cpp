#include "quetzalfieldsdialog.h"
#include "quetzalaccount.h"
#include <QPushButton>
#include <QImageReader>
#include <QBuffer>
#include <QLabel>
#include <QCheckBox>
#include <QSpinBox>
#include <limits>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QComboBox>
#include <QListWidget>
#include <QFormLayout>
#include <qutim/debug.h>

Q_DECLARE_METATYPE(PurpleRequestField*)

typedef QWidget *(*quetzal_create_field)(PurpleRequestField *field, QWidget *parent);

QuetzalFieldsDialog *quetzal_find_super_parent(QWidget *parent)
{
	QuetzalFieldsDialog *dialog = NULL;
	while (parent) {
		if (!!(dialog = qobject_cast<QuetzalFieldsDialog *>(parent)))
			return dialog;
		parent = parent->parentWidget();
	}
	Q_ASSERT(!"Unpossible situation");
	return NULL;
}

QWidget *quetzal_create_string_field(PurpleRequestField *field, QWidget *parent)
{
	QuetzalFieldsDialog *dialog = quetzal_find_super_parent(parent);
	const char *text = purple_request_field_string_get_default_value(field);
	if (purple_request_field_string_is_multiline(field)) {
		QPlainTextEdit *edit = new QPlainTextEdit(text, parent);
		edit->setReadOnly(!purple_request_field_string_is_editable(field));
		QObject::connect(edit, SIGNAL(textChanged(QString)), dialog, SLOT(onStringFieldChanged(QString)));
		return edit;
	} else {
		QLineEdit *edit = new QLineEdit(text, parent);
		if (purple_request_field_string_is_masked(field))
			edit->setEchoMode(QLineEdit::Password);
		edit->setReadOnly(!purple_request_field_string_is_editable(field));
		QObject::connect(edit, SIGNAL(textChanged(QString)), dialog, SLOT(onStringFieldChanged(QString)));
		return edit;
	}
}

QWidget *quetzal_create_integer_field(PurpleRequestField *field, QWidget *parent)
{
	QSpinBox *box = new QSpinBox(parent);
	box->setMaximum(INT_MAX);
	box->setMinimum(INT_MIN);
	box->setValue(purple_request_field_int_get_default_value(field));
	QuetzalFieldsDialog *dialog = quetzal_find_super_parent(parent);
	QObject::connect(box, SIGNAL(valueChanged(int)), dialog, SLOT(onIntFieldChanged(int)));
	return box;
}

QWidget *quetzal_create_boolean_field(PurpleRequestField *field, QWidget *parent)
{
	QCheckBox *box = new QCheckBox(purple_request_field_get_label(field), parent);
	box->setChecked(purple_request_field_bool_get_default_value(field));
	QuetzalFieldsDialog *dialog = quetzal_find_super_parent(parent);
	QObject::connect(box, SIGNAL(toggled(bool)), dialog, SLOT(onBooleanFieldChanged(bool)));
	return box;
}

QWidget *quetzal_create_choice_field(PurpleRequestField *field, QWidget *parent)
{
	QuetzalFieldsDialog *dialog = quetzal_find_super_parent(parent);
	GList *labels = purple_request_field_choice_get_labels(field);
	if (g_list_length(labels) > 5) { // magic number from GTK+ interface
		QComboBox *box = new QComboBox(parent);
		for (; labels; labels = labels->next)
			box->addItem((const char *)labels->data);
		box->setCurrentIndex(purple_request_field_choice_get_default_value(field));
		QObject::connect(box, SIGNAL(currentIndexChanged(int)), dialog, SLOT(onChoiceFieldChanged(int)));
		return box;
	} else {
		QWidget *widget = new QWidget(parent);
		QVBoxLayout *layout = new QVBoxLayout(widget);
		QButtonGroup *group = new QButtonGroup(parent);
		int id = 0;
		int default_id = purple_request_field_choice_get_default_value(field);
		for (; labels; labels = labels->next, id++) {
			QRadioButton *button = new QRadioButton((const char *)labels->data, widget);
			if (id == default_id)
				button->setChecked(true);
			layout->addWidget(button);
			group->addButton(button, id);
		}
		QObject::connect(group, SIGNAL(buttonClicked(int)), dialog, SLOT(onChoiceFieldChanged(int)));
		return widget;
	}
}

QWidget *quetzal_create_list_field(PurpleRequestField *field, QWidget *parent)
{
	QListWidget *list = new QListWidget(parent);
	if (purple_request_field_list_get_multi_select(field))
		list->setSelectionMode(QAbstractItemView::MultiSelection);
	else
		list->setSelectionMode(QAbstractItemView::SingleSelection);
	GList *items = purple_request_field_list_get_items(field);
	for (; items; items = items->next) {
		const char *text = (const char *)items->data;
		QListWidgetItem *item = new QListWidgetItem(text, list);
		item->setData(Qt::UserRole, reinterpret_cast<qptrdiff>(purple_request_field_list_get_data(field, text)));
	}
	QuetzalFieldsDialog *dialog = quetzal_find_super_parent(parent);
	QObject::connect(list, SIGNAL(itemSelectionChanged()), dialog, SLOT(onListFieldChanged()));
	return list;
}

QWidget *quetzal_create_label_field(PurpleRequestField *field, QWidget *parent)
{
	return new QLabel(purple_request_field_get_label(field), parent);
}

QWidget *quetzal_create_image_field(PurpleRequestField *field, QWidget *parent)
{
	QImageReader reader;
	QByteArray data = QByteArray::fromRawData(purple_request_field_image_get_buffer(field),
											  purple_request_field_image_get_size(field));
	QBuffer buffer(&data);
	reader.setDevice(&buffer);
	reader.setScaledSize(QSize(purple_request_field_image_get_scale_x(field),
							   purple_request_field_image_get_scale_y(field)));
	QPixmap pixmap = QPixmap::fromImage(reader.read());
	QLabel *label = new QLabel(parent);
	label->setPixmap(pixmap);
	return label;
}

QWidget *quetzal_create_account_field(PurpleRequestField *field, QWidget *parent)
{
	QComboBox *box = new QComboBox(parent);
	GList *accounts = purple_accounts_get_all_active();
	PurpleFilterAccountFunc filter = purple_request_field_account_get_filter(field);
	for (GList *it = accounts; it; it = it->next) {
		PurpleAccount *acc = (PurpleAccount *)it->data;
		if (filter(acc)) {
			QuetzalAccount *account =  (QuetzalAccount *)acc->ui_data;
			box->addItem(account->status().icon(), account->id(), qVariantFromValue(acc));
		}
	}
	g_list_free(accounts);
	QuetzalFieldsDialog *dialog = quetzal_find_super_parent(parent);
	QObject::connect(box, SIGNAL(currentIndexChanged(int)), dialog, SLOT(onAccountFieldChanged(int)));
	return box;
}

static quetzal_create_field quetzal_field_creators[] = {
	NULL,
	quetzal_create_string_field,
	quetzal_create_integer_field,
	quetzal_create_boolean_field,
	quetzal_create_choice_field,
	quetzal_create_list_field,
	quetzal_create_label_field,
	quetzal_create_image_field,
	quetzal_create_account_field
};

static uint quetzal_field_creators_count = sizeof(quetzal_field_creators) / sizeof(quetzal_create_field);

QuetzalFieldsDialog::QuetzalFieldsDialog(const char *title, const char *primary,
										 const char *secondary, PurpleRequestFields *fields,
										 const char *ok_text, GCallback ok_cb,
										 const char *cancel_text, GCallback cancel_cb,
										 void *user_data, QWidget *parent)
			   : QuetzalRequestDialog(title, primary, secondary, PURPLE_REQUEST_FIELDS, user_data, parent)
{
	m_ok_cb = (PurpleRequestFieldsCb) ok_cb;
	m_cancel_cb = (PurpleRequestFieldsCb) cancel_cb;
	QPushButton *ok_button = buttonBox()->addButton(ok_text, QDialogButtonBox::AcceptRole);
	QPushButton *cancel_button = buttonBox()->addButton(cancel_text, QDialogButtonBox::RejectRole);
	connect(ok_button, SIGNAL(clicked()), this, SLOT(onOkClicked()));
	connect(cancel_button, SIGNAL(clicked()), this, SLOT(onCancelClicked()));
	m_fields = fields;
	GList *group_it = purple_request_fields_get_groups(m_fields), *field_it;
	int index = 1; // Label with description is situated at index 0
	bool useGroupBox = g_list_length(group_it) > 1;
	for (; group_it; group_it = group_it->next) {
		PurpleRequestFieldGroup *group = (PurpleRequestFieldGroup *)group_it->data;
		QWidget *box;
		if (useGroupBox)
			box = new QGroupBox(purple_request_field_group_get_title(group), this);
		else
			box = new QWidget(this);
		QFormLayout *layout = new QFormLayout(box);
		for (field_it = purple_request_field_group_get_fields(group); field_it; field_it = field_it->next) {
			PurpleRequestField *field = (PurpleRequestField *)field_it->data;
			QWidget *widget = (QWidget *)field->ui_data;
			uint type = uint(purple_request_field_get_type(field));
			if (!widget) {
				if (type < quetzal_field_creators_count && quetzal_field_creators[type]) {
					widget = quetzal_field_creators[type](field, box);
				}
				if (!widget)
					continue;
				widget->setObjectName(purple_request_field_get_id(field));
			}
			if (type == PURPLE_REQUEST_FIELD_BOOLEAN)
				layout->addRow(widget);
			else
				layout->addRow(purple_request_field_get_label(field), widget);
			purple_request_field_set_ui_data(field, widget);
		}
		boxLayout()->insertWidget(index++, box);
	}
}

QuetzalFieldsDialog::~QuetzalFieldsDialog()
{
	purple_request_fields_destroy(m_fields);
}

void QuetzalFieldsDialog::onStringFieldChanged(const QString &text)
{
	QByteArray name = sender()->objectName().toUtf8();
	debug() << Q_FUNC_INFO << name << text;
	PurpleRequestField *field = purple_request_fields_get_field(m_fields, name.constData());
	purple_request_field_string_set_value(field, text.toUtf8().constData());
}

void QuetzalFieldsDialog::onIntFieldChanged(int value)
{
	QByteArray name = sender()->objectName().toUtf8();
	PurpleRequestField *field = purple_request_fields_get_field(m_fields, name.constData());
	purple_request_field_int_set_value(field, value);
}

void QuetzalFieldsDialog::onBooleanFieldChanged(bool value)
{
	QByteArray name = sender()->objectName().toUtf8();
	debug() << Q_FUNC_INFO << name << value;
	PurpleRequestField *field = purple_request_fields_get_field(m_fields, name.constData());
	purple_request_field_bool_set_value(field, value);
}

void QuetzalFieldsDialog::onChoiceFieldChanged(int id)
{
	QByteArray name = sender()->objectName().toUtf8();
	PurpleRequestField *field = purple_request_fields_get_field(m_fields, name.constData());
	purple_request_field_choice_set_value(field, id);
}

void QuetzalFieldsDialog::onListFieldChanged()
{
	QListWidget *list = qobject_cast<QListWidget *>(sender());
	QByteArray name = list->objectName().toUtf8();
	PurpleRequestField *field = purple_request_fields_get_field(m_fields, name.constData());
	GList *selected = NULL;
	QList<QByteArray> items;
	foreach (QListWidgetItem *item, list->selectedItems()) {
		items << item->text().toUtf8();
		selected = g_list_append(selected, const_cast<char *>(items.last().constData()));
	}
	debug() << Q_FUNC_INFO << name << items;
	purple_request_field_list_set_selected(field, selected);
	g_list_free(selected);
}

void QuetzalFieldsDialog::onAccountFieldChanged(int id)
{
	QComboBox *box = qobject_cast<QComboBox *>(sender());
	QByteArray name = box->objectName().toUtf8();
	PurpleRequestField *field = purple_request_fields_get_field(m_fields, name.constData());
	purple_request_field_account_set_value(field, box->itemData(id, Qt::UserRole).value<PurpleAccount*>());
}

void QuetzalFieldsDialog::closeRequest()
{
	onCancelClicked();
}

void QuetzalFieldsDialog::onOkClicked()
{
	if (m_ok_cb)
		m_ok_cb(userData(), m_fields);
	m_ok_cb = NULL;
	purple_request_close(PURPLE_REQUEST_FIELDS, this);
}

void QuetzalFieldsDialog::onCancelClicked()
{
	if (m_cancel_cb)
		m_cancel_cb(userData(), m_fields);
	m_cancel_cb = NULL;
	purple_request_close(PURPLE_REQUEST_FIELDS, this);
}

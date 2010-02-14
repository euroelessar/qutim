#include "quetzalinputdialog.h"
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QPushButton>

QuetzalInputDialog::QuetzalInputDialog(const char *title, const char *primary,
									   const char *secondary, const char *default_value,
									   gboolean multiline, gboolean masked, gchar *hint,
									   const char *ok_text, GCallback ok_cb,
									   const char *cancel_text, GCallback cancel_cb,
									   void *user_data, QWidget *parent)
				: QuetzalRequestDialog(title, primary, secondary, PURPLE_REQUEST_INPUT, user_data, parent)
{
	m_ok_cb = (PurpleRequestInputCb) ok_cb;
	m_cancel_cb = (PurpleRequestInputCb) cancel_cb;
	QPushButton *ok_button = buttonBox()->addButton(ok_text, QDialogButtonBox::AcceptRole);
	QPushButton *cancel_button = buttonBox()->addButton(cancel_text, QDialogButtonBox::RejectRole);
	connect(ok_button, SIGNAL(clicked()), this, SLOT(onOkClicked()));
	connect(cancel_button, SIGNAL(clicked()), this, SLOT(onCancelClicked()));
	if (qstrcmp("html", hint)) {
		m_input_widget = new QTextEdit(default_value, this);
		m_property = "html";
	} else if (multiline) {
		m_input_widget = new QPlainTextEdit(default_value, this);
		m_property = "plainText";
	} else {
		m_input_widget = new QLineEdit(default_value, this);
		if (masked)
			static_cast<QLineEdit *>(m_input_widget)->setEchoMode(QLineEdit::Password);
		m_property = "text";
	}
	boxLayout()->insertWidget(1, m_input_widget); // Label with description is situated at index 0
}

void QuetzalInputDialog::closeRequest()
{
	onCancelClicked();
}

void QuetzalInputDialog::onOkClicked()
{
	if (m_ok_cb)
		m_ok_cb(userData(), m_input_widget->property(m_property).toString().toUtf8().constData());
	m_ok_cb = NULL;
	purple_request_close(PURPLE_REQUEST_INPUT, this);
}

void QuetzalInputDialog::onCancelClicked()
{
	if (m_cancel_cb)
		m_cancel_cb(userData(), m_input_widget->property(m_property).toString().toUtf8().constData());
	m_cancel_cb = NULL;
	purple_request_close(PURPLE_REQUEST_INPUT, this);
}

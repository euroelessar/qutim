#include "quetzalchoicedialog.h"
#include <QPushButton>
#include <QVariant>

QuetzalChoiceDialog::QuetzalChoiceDialog(const char *title, const char *primary,
										 const char *secondary, int default_value,
										 const char *ok_text, GCallback ok_cb,
										 const char *cancel_text, GCallback cancel_cb,
										 void *user_data, va_list choices,
										 QWidget *parent)
			   : QuetzalRequestDialog(title, primary, secondary, PURPLE_REQUEST_CHOICE, user_data, parent)
{
	m_ok_cb = (PurpleRequestChoiceCb) ok_cb;
	m_cancel_cb = (PurpleRequestChoiceCb) cancel_cb;
	QPushButton *ok_button = buttonBox()->addButton(ok_text, QDialogButtonBox::AcceptRole);
	QPushButton *cancel_button = buttonBox()->addButton(cancel_text, QDialogButtonBox::RejectRole);
	connect(ok_button, SIGNAL(clicked()), this, SLOT(onOkClicked()));
	connect(cancel_button, SIGNAL(clicked()), this, SLOT(onCancelClicked()));
	const char *text;
	int id;
	int i = 1; // Label with description is situated at index 0
	while (!!(text = va_arg(choices, gchararray))) {
		id = va_arg(choices, int);
		QRadioButton *button = new QRadioButton(text, this);
		m_radios << button;
		button->setProperty("choiceId", id);
		boxLayout()->insertWidget(i++, button);
		if (id == default_value)
			button->setChecked(true);
	}
}

void QuetzalChoiceDialog::closeRequest()
{
	onCancelClicked();
}

void QuetzalChoiceDialog::callBack(PurpleRequestChoiceCb cb)
{
	if (!cb)
		return;
	foreach (QRadioButton *button, m_radios) {
		if (button->isChecked())
			cb(userData(), button->property("choiceId").toInt());
	}
}

void QuetzalChoiceDialog::onOkClicked()
{
	callBack(m_ok_cb);
	m_ok_cb = NULL;
	quetzal_request_close(PURPLE_REQUEST_INPUT, this);
}

void QuetzalChoiceDialog::onCancelClicked()
{
	callBack(m_cancel_cb);
	m_cancel_cb = NULL;
	quetzal_request_close(PURPLE_REQUEST_INPUT, this);
}

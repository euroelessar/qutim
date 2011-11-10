/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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


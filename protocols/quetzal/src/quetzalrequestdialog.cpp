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
#include "quetzalrequestdialog.h"
#include "quetzalrequest.h"
#include <QLabel>

using namespace qutim_sdk_0_3;

QuetzalRequestDialog::QuetzalRequestDialog(const char *title, const char *primary,
										   const char *secondary, PurpleRequestType type,
										   void *user_data, QWidget *parent)
				 : QDialog(parent), m_type(type), m_user_data(user_data)
{
	setWindowTitle(title);
	m_boxLayout = new QVBoxLayout(this);
	QLabel *label = new QLabel(quetzal_create_label(primary, secondary), this);
	label->setWordWrap(true);
	m_boxLayout->addWidget(label);
	m_buttonBox = new QDialogButtonBox(Qt::Horizontal, this);
	m_boxLayout->addWidget(m_buttonBox);
//	connect(m_buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(closeRequest()));
	setAttribute(Qt::WA_QuitOnClose, false);
}

QuetzalRequestDialog::QuetzalRequestDialog(PurpleRequestType type, void *user_data, QWidget *parent)
	: QDialog(parent), m_type(type), m_user_data(user_data)
{
	setAttribute(Qt::WA_QuitOnClose, false);
	m_boxLayout = 0;
	m_buttonBox = 0;
}

void QuetzalRequestDialog::closeRequest()
{
	quetzal_request_close(m_type, this);
}

void QuetzalRequestDialog::onClicked(int)
{
	closeRequest();
}

DataItem QuetzalRequestDialog::createItem(const char *title, const char *primary, const char *secondary)
{
	setWindowTitle(title);
	DataItem item(title);
	{
		DataItem label(LocalizedString(), quetzal_create_label(primary, secondary));
		label.setProperty("hideTitle", true);
		label.setReadOnly(true);
		item.addSubitem(label);
	}
	return item;
}

void QuetzalRequestDialog::createItem(const DataItem &item, const char *okText, const char *cancelText)
{
	AbstractDataForm::Buttons buttons;
	AbstractDataForm::Button okButton = { okText, AbstractDataForm::AcceptRole };
	AbstractDataForm::Button cancelButton = { cancelText, AbstractDataForm::RejectRole };
	buttons << okButton << cancelButton;
	createItem(item, buttons);
}

void QuetzalRequestDialog::createItem(const DataItem &item, const AbstractDataForm::Buttons &buttons)
{
	m_boxLayout = new QVBoxLayout(this);
	m_boxLayout->setMargin(0);
	m_form = AbstractDataForm::get(item, AbstractDataForm::NoButton, buttons);
	m_boxLayout->addWidget(m_form);
	connect(m_form, SIGNAL(clicked(int)), this, SLOT(onClicked(int)));
}

void QuetzalRequestDialog::closeEvent(QCloseEvent *e)
{
	if (!property("quetzal_closed").toBool())
		closeRequest();
	QDialog::closeEvent(e);
}


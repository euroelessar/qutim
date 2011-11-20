/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "simplepassworddialog.h"

namespace Core
{
SimplePasswordDialog::SimplePasswordDialog()
{
}

void SimplePasswordDialog::setAccount(Account *account)
{
	m_widget = new SimplePasswordWidget(account, this);
	connect(this, SIGNAL(destroyed()), m_widget.data(), SLOT(deleteLater()));
	connect(m_widget.data(), SIGNAL(rejected()), this, SIGNAL(rejected()));
	connect(m_widget.data(), SIGNAL(entered(QString,bool)), this, SIGNAL(entered(QString,bool)));
	m_widget.data()->show();
}

void SimplePasswordDialog::setValidator(QValidator *validator)
{
	if (m_widget)
		m_widget.data()->setValidator(validator);
}
}


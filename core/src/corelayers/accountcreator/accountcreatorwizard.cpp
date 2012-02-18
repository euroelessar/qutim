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
#include "accountcreatorwizard.h"
#include <QAction>

namespace Core
{
AccountCreatorWizard::AccountCreatorWizard()
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	AccountCreatorProtocols *protocolsPage = new AccountCreatorProtocols(this);
	setPage(AccountCreatorProtocols::Id, protocolsPage);
	setWindowTitle(tr("Add a new account"));
	resize(800,600);
	setOption(QWizard::NoBackButtonOnStartPage, true);

//#ifdef QUTIM_MOBILE_UI
//	setOption(QWizard::HaveCustomButton1, true);
//	QPushButton *closeBtn = new QPushButton(tr("Close"), this);
//	connect(closeBtn, SIGNAL(clicked()), SLOT(close()));
//	setButton(QWizard::CustomButton1, closeBtn);
//#endif
}
}


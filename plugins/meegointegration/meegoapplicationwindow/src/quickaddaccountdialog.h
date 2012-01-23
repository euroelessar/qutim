/****************************************************************************
**
** qutIM instant messenger
**
** Copyright © 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef QUICKADDACCOUNTDIALOG_H
#define QUICKADDACCOUNTDIALOG_H

#include <QScopedPointer>
#include <qutim/account.h>
#include <qutim/status.h>


namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;

class QuickAddAccountDialog : public QObject {
	Q_OBJECT
	Q_CLASSINFO("Service", "AccountCreator")
	Q_CLASSINFO("Uses", "IconLoader")

public:
	QuickAddAccountDialog();

};
}

#endif /* QUICKADDACCOUNTDIALOG_H*/

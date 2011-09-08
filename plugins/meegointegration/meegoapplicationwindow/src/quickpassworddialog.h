/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef QUICKPASSWORDDIALOG_H
#define QUICKPASSWORDDIALOG_H

#include <qutim/passworddialog.h>

namespace MeegoIntegration
{
using namespace qutim_sdk_0_3;
class QuickPasswordDialog : public PasswordDialog {
	Q_OBJECT

public:
	explicit QuickPasswordDialog();
	virtual void setAccount(Account *account);
	virtual void setValidator(QValidator *validator);
	void accept(const QString &password, bool remember);
	void cancel();
private:
	QString m_templateTitle;

};
}

#endif /* QUICKPASSWORDDIALOG_H*/

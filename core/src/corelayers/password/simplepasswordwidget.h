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
#ifndef SIMPLEPASSWORDWIDGET_H
#define SIMPLEPASSWORDWIDGET_H

#include <QDialog>
#include <QValidator>
#include <qutim/account.h>

namespace Ui {
    class SimplePasswordWidget;
}

namespace Core
{
using namespace qutim_sdk_0_3;
class SimplePasswordDialog;

class SimplePasswordWidget : public QDialog
{
    Q_OBJECT
public:
	SimplePasswordWidget(Account *account, SimplePasswordDialog *parent);
    ~SimplePasswordWidget();

	void setValidator(QValidator *validator);
	void showSaveButton(bool show);
	void showLoginEdit(bool show);
	void setText(const QString &title, const QString &description);

protected:
    void changeEvent(QEvent *e);
public slots:
	void onAccept();
	void onReject();
signals:
	void entered(const QString &password, bool remember);

private:
	Account *m_account;
	SimplePasswordDialog *m_parent;
    Ui::SimplePasswordWidget *ui;
};
}

#endif // SIMPLEPASSWORDWIDGET_H


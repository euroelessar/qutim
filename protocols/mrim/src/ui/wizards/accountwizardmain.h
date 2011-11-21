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
#ifndef ACCOUNTWIZARDMAIN_H
#define ACCOUNTWIZARDMAIN_H

#include <QWizardPage>

namespace Ui {
    class AccountWizardMain;
}

class AccountWizardMain : public QWizardPage
{
    Q_OBJECT
public:
    AccountWizardMain(QWidget *parent = 0);
    ~AccountWizardMain();

protected:
    void changeEvent(QEvent *e);
    bool validatePage();

private:
    Ui::AccountWizardMain *ui;
};

#endif // ACCOUNTWIZARDMAIN_H


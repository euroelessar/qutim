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
#ifndef ACCOUNTCREATORLIST_H
#define ACCOUNTCREATORLIST_H

#include <qutim/settingslayer.h>
#include <qutim/settingswidget.h>
#include "accountcreatorwizard.h"

namespace Ui {
    class AccountCreatorList;
}

namespace qutim_sdk_0_3
{
	class Account;
}

class QListWidgetItem;
namespace Core
{
using namespace qutim_sdk_0_3;
class AccountCreatorList : public SettingsWidget
{
	Q_OBJECT
public:
	AccountCreatorList();
	~AccountCreatorList();
protected:
	virtual void loadImpl();
	virtual void saveImpl();
	virtual void cancelImpl();
	void changeEvent(QEvent *e);
private slots:
	void addAccount(qutim_sdk_0_3::Account *account);
	void removeAccount(qutim_sdk_0_3::Account *account);
	void listViewClicked(QListWidgetItem *item);
	void onWizardDestroyed();
	void onAccountRemoveTriggered();
	void onAccountPropertiesTriggered();
private:
	Ui::AccountCreatorList *ui;
	QPointer<AccountCreatorWizard> m_wizard;
};
}

#endif // ACCOUNTCREATORLIST_H


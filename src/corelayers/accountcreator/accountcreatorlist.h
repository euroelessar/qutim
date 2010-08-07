#ifndef ACCOUNTCREATORLIST_H
#define ACCOUNTCREATORLIST_H

#include "libqutim/settingslayer.h"
#include "libqutim/settingswidget.h"
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
		virtual bool eventFilter(QObject *, QEvent *);
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

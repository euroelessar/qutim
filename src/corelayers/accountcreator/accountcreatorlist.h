#ifndef ACCOUNTCREATORLIST_H
#define ACCOUNTCREATORLIST_H

#include "libqutim/settingslayer.h"
#include "libqutim/settingswidget.h"

namespace Ui {
    class AccountCreatorList;
}
using namespace qutim_sdk_0_3;

namespace Core
{
	class AccountCreatorList : public SettingsWidget
	{
		Q_OBJECT
	public:
		AccountCreatorList();
		~AccountCreatorList();

	public slots:
		void on_addButton_clicked();

	protected:
		virtual void loadImpl();
		virtual void saveImpl();
		virtual void cancelImpl();
		void changeEvent(QEvent *e);

	private:
		Ui::AccountCreatorList *m_ui;
	};
}

#endif // ACCOUNTCREATORLIST_H

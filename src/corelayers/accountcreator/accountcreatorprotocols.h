#ifndef ACCOUNTCREATORPROTOCOLS_H
#define ACCOUNTCREATORPROTOCOLS_H

#include <QtGui/QWizardPage>
#include "libqutim/protocol.h"

using namespace qutim_sdk_0_3;

namespace Ui {
    class AccountCreatorProtocols;
}

class QListWidgetItem;

namespace Core
{
	class AccountCreatorWizard;

	class AccountCreatorProtocols : public QWizardPage
	{
		Q_OBJECT
	public:
		enum { Id = 1 };
		AccountCreatorProtocols(AccountCreatorWizard *parent = 0);
		~AccountCreatorProtocols();
		virtual bool validatePage();
		virtual bool isComplete() const;
		virtual int nextId() const;

	public slots:
		void protocolSelected();
		void on_upButton_clicked();
		void on_downButton_clicked();
		void sliderMoved(int val);

	protected:
		QMap<AccountCreationWizard *, int>::iterator ensureCurrentProtocol();
		void changeEvent(QEvent *e);

	private:
		Ui::AccountCreatorProtocols *m_ui;
		AccountCreatorWizard *m_wizard;
		QMultiMap<QString, AccountCreationWizard *> m_wizards;
		QMap<AccountCreationWizard *, int> m_wizardIds;
		QHash<QPushButton *, QListWidgetItem *> m_items;
		int m_lastId;
	};
}

#endif // ACCOUNTCREATORPROTOCOLS_H

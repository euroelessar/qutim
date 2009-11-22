#ifndef ACCOUNTCREATORPROTOCOLS_H
#define ACCOUNTCREATORPROTOCOLS_H

#include <QtGui/QWizardPage>
#include "libqutim/protocol.h"

using namespace qutim_sdk_0_3;

namespace Ui {
    class AccountCreatorProtocols;
}

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
		void on_protocolsBox_currentIndexChanged(int index);

	protected:
		void changeEvent(QEvent *e);

	private:
		Ui::AccountCreatorProtocols *m_ui;
		AccountCreatorWizard *m_wizard;
	};
}

#endif // ACCOUNTCREATORPROTOCOLS_H

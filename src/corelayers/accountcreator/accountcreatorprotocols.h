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
	class AccountCreatorProtocols : public QWizardPage {
		Q_OBJECT
	public:
		AccountCreatorProtocols(QWidget *parent = 0);
		~AccountCreatorProtocols();

	public slots:
		void on_protocolsBox_currentIndexChanged(int index);

	protected:
		void changeEvent(QEvent *e);

	private:
		Ui::AccountCreatorProtocols *m_ui;
	};
}

#endif // ACCOUNTCREATORPROTOCOLS_H

#ifndef JACCOUNTWIZARDPAGE_H
#define JACCOUNTWIZARDPAGE_H

#include <QWizardPage>
#include "jaccountwizard.h"

namespace Ui
{
	class JAccountWizardPage;
}

namespace Jabber
{
	class JAccountWizardPage : public QWizardPage
	{
		Q_OBJECT
		public:
			JAccountWizardPage(JAccountWizard *accountWizard,
					JAccountType type, QWidget *parent = 0);
			~JAccountWizardPage();
			bool validatePage();
			QString jid();
			QString passwd();
			bool isSavePasswd();
		private:
			JAccountWizard *m_accountWizard;
			JAccountType m_type;
			Ui::JAccountWizardPage *ui;
	};
}

#endif // JACCOUNTWIZARDPAGE_H

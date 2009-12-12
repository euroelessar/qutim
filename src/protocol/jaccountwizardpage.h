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
				JAccountType type);
			~JAccountWizardPage();
			bool validatePage();
		protected:
			Ui::JAccountWizardPage *ui;
		private:
			JAccountWizard *m_accountWizard;
	};
}

#endif // JACCOUNTWIZARDPAGE_H

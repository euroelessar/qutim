#ifndef JACCOUNTWIZARDPAGE_H
#define JACCOUNTWIZARDPAGE_H

#include <QtGui/QWizardPage>
#include "jaccountwizard.h"
#include "ui_jaccountwizardpage.h"

//namespace Ui
//{
//	class JAccountWizardPage;
//}

namespace Jabber
{
	class JAccountWizard;

	class JAccountWizardPage : public QWizardPage
	{
		Q_OBJECT

		public:
			JAccountWizardPage(JAccountWizard *accountWizard,
				JAccountWizard::JAccountType type);
			~JAccountWizardPage();
			bool validatePage();
		protected:
			Ui::JAccountWizardPage *ui;
		private:
			JAccountWizard *m_accountWizard;
	};
}

#endif // JACCOUNTWIZARDPAGE_H

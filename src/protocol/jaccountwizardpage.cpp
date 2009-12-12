#include "jaccountwizardpage.h"
#include "ui_jaccountwizardpage.h"

namespace Jabber
{
	JAccountWizardPage::JAccountWizardPage(JAccountWizard *accountWizard,
		JAccountType type)
		: m_accountWizard(accountWizard)
	{
		ui->setupUi(this);
		switch (type) {
		case AccountTypeJabber:
			ui->serverLabel->setVisible(false);
			break;
		case AccountTypeLivejournal:
			ui->serverLabel->setText("@livejournal.com");
			break;
		case AccountTypeYandex:
			ui->serverLabel->setText("@ya.ru");
			break;
		case AccountTypeGoogletalk:
			ui->serverLabel->setText("@google.com");
			break;
		case AccountTypeQip:
			ui->serverLabel->setText("@qip.ru");
			break;
		}
	}

	JAccountWizardPage::~JAccountWizardPage()
	{
		delete ui;
	}

	bool JAccountWizardPage::validatePage()
	{
		m_accountWizard->createAccount();
		return true;
	}
}

#include "jaccountwizardpage.h"

namespace Jabber
{
	JAccountWizardPage::JAccountWizardPage(JAccountWizard *accountWizard,
		JAccountWizard::JAccountType type)
		: m_accountWizard(accountWizard)
	{
		ui->setupUi(this);
		switch (type) {
		case JAccountWizard::JABBER:
			ui->serverLabel->setVisible(false);
			break;
		case JAccountWizard::LIVEJOURNAL:
			ui->serverLabel->setText("@livejournal.com");
			break;
		case JAccountWizard::YANDEX:
			ui->serverLabel->setText("@ya.ru");
			break;
		case JAccountWizard::GOOGLETALK:
			ui->serverLabel->setText("@google.com");
			break;
		case JAccountWizard::QIP:
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

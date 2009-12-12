#include "jaccountwizardpage.h"

JAccountWizardPage::JAccountWizardPage(QWidget *parent) : QWizardPage(parent)
{
	ui.setupUi(this);
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

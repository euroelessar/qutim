#include "jaccountwizardpage.h"
#include "ui_jaccountwizardpage.h"

namespace Jabber
{
	JAccountWizardPage::JAccountWizardPage(JAccountWizard *accountWizard,
		JAccountType type)
		: m_accountWizard(accountWizard), ui(new Ui::JAccountWizardPage), m_type(type)
	{
		ui->setupUi(this);
		QRegExp rx("[a-z1-9\\._-\\+]+@[a-z1-9_-]+\\.[a-z]{2,}");
		switch (m_type) {
		case AccountTypeJabber:
			ui->serverLabel->setVisible(false);
			break;
		case AccountTypeLivejournal:
			ui->serverLabel->setText("@livejournal.com");
			rx = QRegExp("[a-z1-9\\._-\\+]+(@livejournal.com)?");
			break;
		case AccountTypeYandex:
			ui->serverLabel->setText("@ya.ru");
			rx = QRegExp("[a-z1-9\\._-\\+]+(@ya.ru)?");
			break;
		case AccountTypeGoogletalk:
			ui->serverLabel->setText("@gmail.com");
			rx = QRegExp("[a-z1-9\\._-\\+]+(@gmail.com)?");
			break;
		case AccountTypeQip:
			ui->serverLabel->setText("@qip.ru");
			rx = QRegExp("[a-z1-9\\._-\\+]+(@qip.ru)?");
			break;
		}
		QValidator *validator = new QRegExpValidator(rx, this);
		ui->jidEdit->setValidator(validator);
	}

	JAccountWizardPage::~JAccountWizardPage()
	{
		delete ui;
	}

	bool JAccountWizardPage::validatePage()
	{
		if (jid().isEmpty() || (isSavePasswd() && passwd().isEmpty()))
			return false;
		if (m_type > AccountTypeJabber)
		{
			QRegExp rx("@\\.*");
			ui->jidEdit->setText(ui->jidEdit->text().remove(rx) + ui->serverLabel->text());
		}
		m_accountWizard->createAccount();
		return true;
	}
	
	QString JAccountWizardPage::jid()
	{
		return ui->jidEdit->text();
	}
	
	QString JAccountWizardPage::passwd()
	{
		return ui->passwdEdit->text();
	}
	
	bool JAccountWizardPage::isSavePasswd()
	{
		return ui->savePasswdCheck->isChecked();
	}
}

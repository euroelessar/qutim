#include "jaccountwizard.h"

namespace Jabber
{

	JAccountWizard::JAccountWizard() : AccountCreationWizard(JProtocol::instance())
	{
		type = JABBER;
	}

	JAccountWizard::~JAccountWizard()
	{
	}

	QList<QWizardPage *> JAccountWizard::createPages(QWidget *parent)
	{
		page = new JAccountWizardPage(this);
		switch (type) {
		case JABBER:
			page->ui->serverLabel->setVisible(false);
			break;
		case LIVEJOURNAL:
			page->ui->serverLabel->setText("@livejournal.com");
			break;
		case YANDEX:
			page->ui->serverLabel->setText("@ya.ru");
			break;
		case GOOGLETALK:
			page->ui->serverLabel->setText("@google.com");
			break;
		case QIP:
			page->ui->serverLabel->setText("@qip.ru");
			break;
		}
	}

	void JAccountWizard::createAccount()
	{
	}

	LJAccountWizard::LJAccountWizard()
	{
		type = JAccountWizard::LIVEJOURNAL;
	}

	LJAccountWizard::~LJAccountWizard()
	{
	}

	GTAccountWizard::GTAccountWizard()
	{
		type = JAccountWizard::GOOGLETALK;
	}

	GTAccountWizard::~GTAccountWizard()
	{
	}

	YAccountWizard::YAccountWizard()
	{
		JAccountWizard::YANDEX;
	}

	YAccountWizard::~YAccountWizard()
	{
	}

	QIPAccountWizard::QIPAccountWizard()
	{
		JAccountWizard::QIP;
	}

	QIPAccountWizard::~QIPAccountWizard()
	{
	}
}

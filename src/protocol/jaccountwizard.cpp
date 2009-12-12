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
		page = new JAccountWizardPage(this, type);
	}

	void JAccountWizard::createAccount()
	{
	}

	LJAccountWizard::LJAccountWizard()
	{
		type = JAccountWizard::LIVEJOURNAL;
		ExtensionInfo info("LiveJournal", "Add LiveJournal account");
		setInfo(info);
	}

	LJAccountWizard::~LJAccountWizard()
	{
	}

	GTAccountWizard::GTAccountWizard()
	{
		type = JAccountWizard::GOOGLETALK;
		ExtensionInfo info("GoogleTalk", "Add GoogleTalk account");
		setInfo(info);
	}

	GTAccountWizard::~GTAccountWizard()
	{
	}

	YAccountWizard::YAccountWizard()
	{
		type = JAccountWizard::YANDEX;
		ExtensionInfo info("Yandex.Online", "Add Yandex.Online account");
		setInfo(info);
	}

	YAccountWizard::~YAccountWizard()
	{
	}

	QIPAccountWizard::QIPAccountWizard()
	{
		type = JAccountWizard::QIP;
		ExtensionInfo info("QIP", "Add QIP account");
		setInfo(info);
	}

	QIPAccountWizard::~QIPAccountWizard()
	{
	}
}

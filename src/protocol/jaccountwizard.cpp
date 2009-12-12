#include "jaccountwizard.h"
#include "jaccountwizardpage.h"

namespace Jabber
{

	JAccountWizard::JAccountWizard() : AccountCreationWizard(JProtocol::instance())
	{
		type = AccountTypeJabber;
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
		type = AccountTypeLivejournal;
		ExtensionInfo info("LiveJournal", "Add LiveJournal account");
		setInfo(info);
	}

	LJAccountWizard::~LJAccountWizard()
	{
	}

	GTAccountWizard::GTAccountWizard()
	{
		type = AccountTypeGoogletalk;
		ExtensionInfo info("GoogleTalk", "Add GoogleTalk account");
		setInfo(info);
	}

	GTAccountWizard::~GTAccountWizard()
	{
	}

	YAccountWizard::YAccountWizard()
	{
		type = AccountTypeYandex;
		ExtensionInfo info("Yandex.Online", "Add Yandex.Online account");
		setInfo(info);
	}

	YAccountWizard::~YAccountWizard()
	{
	}

	QIPAccountWizard::QIPAccountWizard()
	{
		type = AccountTypeQip;
		ExtensionInfo info("QIP", "Add QIP account");
		setInfo(info);
	}

	QIPAccountWizard::~QIPAccountWizard()
	{
	}
}

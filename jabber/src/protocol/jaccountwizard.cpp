#include "jaccountwizard.h"
#include "jaccountwizardpage.h"
#include "jprotocol.h"
#include "account/jaccount.h"

namespace Jabber
{

	JAccountWizard::JAccountWizard() : AccountCreationWizard(JProtocol::instance())
	{
		protocol = JProtocol::instance();
		type = AccountTypeJabber;
	}

	JAccountWizard::~JAccountWizard()
	{
	}

	QList<QWizardPage *> JAccountWizard::createPages(QWidget *parent)
	{
		page = new JAccountWizardPage(this, type, parent);
		QList<QWizardPage *> pages;
		pages.append(page);
		return pages;
	}

	void JAccountWizard::createAccount()
	{
		JAccount *account = new JAccount(page->jid());
		if(page->isSavePasswd())
		{
			account->config().group("general").setValue("passwd", page->passwd(), Config::Crypted);
			account->config().sync();
		}
		ConfigGroup cfg = protocol->config().group("general");
		QStringList accounts = cfg.value("accounts", QStringList());
		accounts << account->id();
		cfg.setValue("accounts", accounts);
		cfg.sync();
		protocol->addAccount(account, true);
		delete page;
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

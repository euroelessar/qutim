#include "jaccountwizardpage.h"
#include "ui_jaccountwizardpage.h"
#include <gloox/jid.h>
#include <gloox/gloox.h>

namespace Jabber
{
	class JJidValidatorPrivate
	{
	public:
		std::string server;
	};

	JJidValidator::JJidValidator(const QString &server, QObject *parent)
			: QValidator(parent), d_ptr(new JJidValidatorPrivate)
	{
		d_func()->server = server.isNull() ? gloox::EmptyString : server.toStdString();
	}

	JJidValidator::JJidValidator(const std::string &server, QObject *parent)
			: QValidator(parent), d_ptr(new JJidValidatorPrivate)
	{
		d_func()->server = server;
	}

	JJidValidator::~JJidValidator()
	{
	}

	QString JJidValidator::server() const
	{
		return QString::fromStdString(d_func()->server);
	}

	QValidator::State JJidValidator::validate(QString &str, int &) const
	{
		Q_D(const JJidValidator);
		gloox::JID jid;
		if (d->server.empty()) {
			std::string jidFull = str.toStdString();
			jid.setJID(jidFull);
			if (!jid)
				return Invalid;
			if (jid.full() == jidFull)
				return Acceptable;
		} else {
			std::string jidNode = str.toStdString();
			if (str.indexOf('@') != -1) {
				jid.setJID(jidNode);
				jid.setServer(d->server);
			} else {
				jid.setUsername(jidNode);
				jid.setServer(d->server);
			}
			if (!jid)
				return Invalid;
		}
		fixup(str);
		return Acceptable;
	}

	void JJidValidator::fixup(QString &str) const
	{
		Q_D(const JJidValidator);
		gloox::JID jid;
		if (d->server.empty()) {
			jid.setJID(str.toStdString());
			str = QString::fromStdString(jid.bare());
		} else {
			std::string jidNode = str.toStdString();
			if (str.indexOf('@') != -1) {
				jid.setJID(jidNode);
				jid.setServer(d->server);
			} else {
				jid.setUsername(jidNode);
				jid.setServer(d->server);
			}
			str = QString::fromStdString(jid.username());
		}
	}

	JAccountWizardPage::JAccountWizardPage(JAccountWizard *accountWizard, JAccountType type, QWidget *parent)
		: QWizardPage(parent), m_accountWizard(accountWizard), ui(new Ui::JAccountWizardPage), m_type(type)
	{
		ui->setupUi(this);
		QString server;
//		QRegExp rx("[a-z1-9\\._-\\+]+@[a-z1-9_-]+\\.[a-z]{2,}");
		switch (m_type) {
		case AccountTypeJabber:
//			ui->serverLabel->setVisible(false);
			break;
		case AccountTypeLivejournal:
//			ui->serverLabel->setText("@livejournal.com");
//			rx = QRegExp("[a-z1-9\\._-\\+]+(@livejournal.com)?");
			server = "livejournal.com";
			break;
		case AccountTypeYandex:
//			ui->serverLabel->setText("@ya.ru");
//			rx = QRegExp("[a-z1-9\\._-\\+]+(@ya.ru)?");
			server = "ya.ru";
			break;
		case AccountTypeGoogletalk:
//			ui->serverLabel->setText("@gmail.com");
//			rx = QRegExp("[a-z1-9\\._-\\+]+(@gmail.com)?");
//			server = "gmail.com";
			break;
		case AccountTypeQip:
//			ui->serverLabel->setText("@qip.ru");
//			rx = QRegExp("[a-z1-9\\._-\\+]+(@qip.ru)?");
			server = "qip.ru";
			break;
		}
		if (!server.isEmpty())
			ui->serverLabel->setText("@" + server);
		else
			ui->serverLabel->hide();
		QValidator *validator = new JJidValidator(server, this);
		ui->jidEdit->setValidator(validator);
		registerField("jid", ui->jidEdit);
		registerField("password", ui->passwdEdit);
		registerField("savePassword", ui->savePasswdCheck);
	}

	JAccountWizardPage::~JAccountWizardPage()
	{
		delete ui;
	}

	bool JAccountWizardPage::validatePage()
	{
		if (jid().isEmpty() || (isSavePasswd() && passwd().isEmpty()))
			return false;
//		if (m_type > AccountTypeJabber)
//		{
//			QRegExp rx("@\\.*");
//			ui->jidEdit->setText(ui->jidEdit->text().remove(rx) + ui->serverLabel->text());
//		}
		m_accountWizard->createAccount();
		return true;
	}
	
	QString JAccountWizardPage::jid()
	{
		const JJidValidator *validator = qobject_cast<const JJidValidator *>(ui->jidEdit->validator());
		QString server = validator->server();
		if (server.isEmpty())
			return ui->jidEdit->text();
		else
			return ui->jidEdit->text() + "@" + server;
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

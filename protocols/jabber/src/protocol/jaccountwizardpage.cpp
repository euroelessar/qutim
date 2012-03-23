/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#include "jaccountwizardpage.h"
#include "ui_jaccountwizardpage.h"
#include "jjidvalidator.h"
#include <QNetworkReply>
#include <jreen/jid.h>
#include <qutim/debug.h>

namespace Jabber
{

using namespace Jreen;

JAccountWizardPage::JAccountWizardPage(JAccountWizard *accountWizard, JAccountType type, QWidget *parent)
	: QWizardPage(parent), m_accountWizard(accountWizard), m_type(type), ui(new Ui::JAccountWizardPage)
{
	ui->setupUi(this);
	setSubTitle(tr("Account information.\nIf you don't have account yet, press \"Register\" button."));
	QString server;
	switch (m_type) {
	case AccountTypeJabber:
		break;
	case AccountTypeLivejournal:
		server = "livejournal.com";
		break;
	case AccountTypeYandex:
		server = "ya.ru";
		break;
	case AccountTypeGoogletalk:
//		ui->serverEdit->addItem(QLatin1String("gmail.com"));
		break;
	case AccountTypeQip:
		server = "qip.ru";
		break;
	}
	if (!server.isEmpty()) {
		ui->serverLabel->setText("@" + server);
		ui->groupBox_2->hide();
		ui->serverEdit->addItem(server);
		ui->serverEdit->setEditText(server);
	} else {
		ui->serverLabel->hide();
	}
	QValidator *validator = new JJidValidator(server, this);
	ui->jidEdit->setValidator(validator);
	registerField("server", ui->serverEdit, "currentText", SIGNAL(editTextChanged(QString)));
	registerField("jid", ui->jidEdit);
	registerField("password", ui->passwdEdit);
	registerField("savePassword", ui->savePasswdCheck);
	setButtonText(QWizard::CustomButton1, tr("Register"));
	connect(&m_networkManager, SIGNAL(finished(QNetworkReply*)), SLOT(onFinished(QNetworkReply*)));
}

JAccountWizardPage::~JAccountWizardPage()
{
	delete ui;
}

bool JAccountWizardPage::validatePage()
{
	if (ui->newAccountButton->isChecked()) {
		return !ui->serverEdit->currentText().isEmpty();
	} else {
		if (jid().isEmpty() || (isSavePasswd() && passwd().isEmpty()))
			return false;
		m_accountWizard->createAccount();
		return true;
	}
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

int JAccountWizardPage::nextId() const
{
	if (ui->newAccountButton->isChecked())
		return wizard()->currentId() + 1;
	else
		return -1;
}

void JAccountWizardPage::on_newAccountButton_clicked()
{
	if (ui->serverEdit->count() == 0) {
		QUrl url(QLatin1String("http://xmpp.net/services.xml"));
		QNetworkRequest request(url);
		m_networkManager.get(request);
	}
    setFinalPage(false);
}

void JAccountWizardPage::on_oldAccountButton_clicked()
{
	setFinalPage(true);
}

void JAccountWizardPage::onFinished(QNetworkReply *reply)
{
	reply->deleteLater();
	QByteArray data = reply->readAll();
	QXmlStreamReader reader(data);
	QStringList servers;
	while (!reader.atEnd()) {
		if (reader.readNextStartElement()) {
			if (reader.name() != QLatin1String("item"))
				continue;
			QStringRef jid = reader.attributes().value(QLatin1String("jid"));
			if (!jid.isEmpty())
				servers << jid.toString();
		}
    }
	QString text = ui->serverEdit->currentText();
	if (text.isEmpty()) {
		int index = qrand() % servers.size();
		text = servers.value(index);
	}
	ui->serverEdit->addItems(servers);
	ui->serverEdit->setEditText(text);
}
}

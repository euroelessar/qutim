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
#include <jreen/jid.h>
#include <qutim/debug.h>

namespace Jabber
{

using namespace Jreen;

JAccountWizardPage::JAccountWizardPage(JAccountWizard *accountWizard, JAccountType type, QWidget *parent)
	: QWizardPage(parent), m_accountWizard(accountWizard), m_type(type), ui(new Ui::JAccountWizardPage)
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
	qutim_sdk_0_3::debug() << Q_FUNC_INFO << " " << jid() << " " << passwd();
	if (jid().isEmpty() || (isSavePasswd() && passwd().isEmpty()))
		return false;
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


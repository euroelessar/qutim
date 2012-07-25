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
#include "vaccountcreator.h"
#include "ui_vaccountwizardpage.h"
#include "../vaccount.h"
#include "vprotocol.h"
#include <qutim/extensioninfo.h>

using namespace qutim_sdk_0_3;

class VAccountWizardPage: public QWizardPage
{
public:
	VAccountWizardPage(VAccountCreator *account_wizard);
	bool validatePage();
	QString email() { return ui.emailEdit->text(); }
	QString password() { return ui.passwdEdit->text(); }
	bool isSavePassword() { return ui.savePasswdCheck->isChecked(); }
private:
	VAccountCreator *m_account_wizard;
	Ui::VAccountWizardPage ui;
};

VAccountWizardPage::VAccountWizardPage(VAccountCreator* account_wizard) : m_account_wizard(account_wizard)
{
	ui.setupUi(this);
	{
		//TODO email validator
		// 		QRegExp rx("[1-9][0-9]{1,9}");
		// 		QValidator *validator = new QRegExpValidator(rx, this);
		// 		ui.emailEdit->setValidator(validator);
	}
	ui.emailEdit->setFocus();
}
bool VAccountWizardPage::validatePage()
{
	if (email().isEmpty() || (isSavePassword() && password().isEmpty()))
		return false;
	m_account_wizard->finished();
	return true;
}



VAccountCreator::VAccountCreator() : AccountCreationWizard(VProtocol::instance()),
    m_page(0)
{
	ExtensionInfo info(QT_TRANSLATE_NOOP("Protocol", "vk.com"),
					   QT_TRANSLATE_NOOP("Protocol", "Add vk.com account"));
	setInfo(info);
}

VAccountCreator::~VAccountCreator()
{

}

QList< QWizardPage* > VAccountCreator::createPages(QWidget* parent)
{
	Q_UNUSED(parent);
	m_page = new VAccountWizardPage(this);
	QList<QWizardPage *> pages;
	pages << m_page;
	return pages;
}

void VAccountCreator::finished()
{
    VProtocol *protocol = VProtocol::instance();
    VAccount *account = new VAccount(m_page->email(), protocol);
	if (m_page->isSavePassword()) {
		ConfigGroup cfg = account->config().group("general");
		cfg.setValue("passwd", m_page->password(), Config::Crypted);
		cfg.sync();
	}
    ConfigGroup cfg = protocol->config().group("general");
	QStringList accounts = cfg.value("accounts", QStringList());
	accounts << account->id();
	cfg.setValue("accounts", accounts);
	cfg.sync();
    protocol->addAccount(account);
	m_page->deleteLater();
}


/****************************************************************************
 *  accountcreator.cpp
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
 *****************************************************************************/

#include "accountcreator.h"
#include "ircprotocol.h"
#include "ircaccount.h"
#include <QWizard>
#include <QRegExp>
#include <QValidator>
#include <qutim/icon.h>
#include "ui_addaccountform.h"
#include "ui_editnickform.h"

namespace qutim_sdk_0_3 {

namespace irc {

IrcAccWizardPage::IrcAccWizardPage(IrcAccountCreationWizard *accountWizard, QWidget *parent) :
	QWizardPage(parent), m_accountWizard(accountWizard), ui(new Ui::AddAccountForm)
{
	ui->setupUi(this);
	ui->addServerButton->setIcon(Icon("list-add-server-irc"));
	ui->removeServerButton->setIcon(Icon("list-remove-server-irc"));
	ui->updateServerButton->setIcon(Icon("document-edit-server-irc"));
	ui->moveUpServerButton->setIcon(Icon("arrow-up-server-irc"));
	ui->moveDownServerButton->setIcon(Icon("arrow-down-server-irc"));
	connect(ui->addServerButton, SIGNAL(clicked()), SLOT(onAddServer()));
	connect(ui->updateServerButton, SIGNAL(clicked()), SLOT(onEditServer()));
	connect(ui->removeServerButton, SIGNAL(clicked()), SLOT(onRemoveServer()));
	connect(ui->moveUpServerButton, SIGNAL(clicked()), SLOT(onMoveUpServer()));
	connect(ui->moveDownServerButton, SIGNAL(clicked()), SLOT(onMoveDownServer()));
	connect(ui->serversWidget, SIGNAL(currentRowChanged(int)), SLOT(onCurrentServerChanged(int)));
	connect(ui->protectedByPasswordBox, SIGNAL(toggled(bool)), SLOT(onPasswordProtectionChanged(bool)));

	QListWidgetItem *item = new QListWidgetItem("New server", ui->serversWidget);
	ui->serversWidget->addItem(item);

	ui->networkEdit->setFocus();
}

IrcAccWizardPage::~IrcAccWizardPage()
{
	delete ui;
}

QString IrcAccWizardPage::networkName() const
{
	return ui->networkEdit->text();
}

bool IrcAccWizardPage::validatePage()
{
	QString network = networkName();
	if (network.isEmpty() || IrcProtocol::instance()->account(network) || m_servers.isEmpty())
		return false;
	return true;
}

void IrcAccWizardPage::onAddServer()
{
	ServerData server = currentServer();
	QListWidgetItem *item = new QListWidgetItem(QString("%1:%2").arg(server.hostName).arg(server.port), ui->serversWidget);
	QListWidgetItem *nullItem = ui->serversWidget->takeItem(m_servers.size());
	ui->serversWidget->insertItem(m_servers.size(), item);
	ui->serversWidget->addItem(nullItem);
	m_servers.push_back(server);
	ui->serversWidget->setCurrentItem(item);
}

void IrcAccWizardPage::onEditServer()
{
	int row = ui->serversWidget->currentRow();
	Q_ASSERT(row >= 0 && row < m_servers.size());
	ServerData server = currentServer();
	m_servers[row] = server;
	ui->serversWidget->currentItem()->setText(QString("%1:%2").arg(server.hostName).arg(server.port));
}

void IrcAccWizardPage::onRemoveServer()
{
	int row = ui->serversWidget->currentRow();
	Q_ASSERT(row >= 0 && row < m_servers.size());
	m_servers.removeAt(row);
	delete ui->serversWidget->currentItem();
}

void IrcAccWizardPage::onMoveUpServer()
{
	int row = ui->serversWidget->currentRow();
	moveServer(row, row-1);
}

void IrcAccWizardPage::onMoveDownServer()
{
	int row = ui->serversWidget->currentRow();
	moveServer(row, row+1);
}

inline ServerData IrcAccWizardPage::currentServer()
{
	ServerData server;
	server.hostName = ui->serverEdit->text();
	server.port = ui->portBox->value();
	server.protectedByPassword = ui->protectedByPasswordBox->isChecked();
	server.password = ui->passwordEdit->text();
	server.savePassword = ui->passwordBox->isChecked();
	return server;
}

void IrcAccWizardPage::moveServer(int row, int newRow)
{
	Q_ASSERT(row >= 0 && row < m_servers.size());
	Q_ASSERT(newRow >= 0 && newRow < m_servers.size());
	QListWidgetItem *item = ui->serversWidget->takeItem(row);
	ui->serversWidget->insertItem(newRow, item);
	ServerData server = m_servers.takeAt(row);
	m_servers.insert(newRow, server);
	ui->serversWidget->setCurrentItem(item);
}

void IrcAccWizardPage::onCurrentServerChanged(int row)
{
	int serversCount = m_servers.size();
	bool isServerChecked = row >= 0 && row < serversCount;
	if (isServerChecked) {
		ServerData server = m_servers.at(row);
		ui->serverEdit->setText(server.hostName);
		ui->portBox->setValue(server.port);
		ui->protectedByPasswordBox->setChecked(server.protectedByPassword);
		ui->passwordEdit->setText(server.password);
		ui->passwordBox->setChecked(server.savePassword);
	} else {
		ui->serverEdit->clear();
		ui->portBox->setValue(6667);
		ui->protectedByPasswordBox->setChecked(false);
		ui->passwordEdit->clear();
		ui->passwordBox->setChecked(false);
	}
	ui->updateServerButton->setEnabled(isServerChecked);
	ui->removeServerButton->setEnabled(isServerChecked);
	ui->moveUpServerButton->setEnabled(row >= 1 && row < serversCount);
	ui->moveDownServerButton->setEnabled(row >= 0 && row < serversCount-1);
}

void IrcAccWizardPage::onPasswordProtectionChanged(bool checked)
{
	ui->passwordBox->setEnabled(checked);
	ui->passwordEdit->setEnabled(checked);
	ui->passwordLabel->setEnabled(checked);
}

IrcNickWizardPage::IrcNickWizardPage(IrcAccountCreationWizard *accountWizard, QWidget *parent) :
	QWizardPage(parent), ui(new Ui::EditNickForm), m_accountWizard(accountWizard)
{
	ui->setupUi(this);
	ui->addNickButton->setIcon(Icon("list-add-nick-irc"));
	ui->removeNickButton->setIcon(Icon("list-remove-nick-irc"));
	ui->updateNickButton->setIcon(Icon("document-edit-nick-irc"));
	ui->moveNickUpButton->setIcon(Icon("arrow-up-nick-irc"));
	ui->moveNickDownButton->setIcon(Icon("arrow-down-nick-irc"));
	connect(ui->addNickButton, SIGNAL(clicked()), SLOT(onAddNick()));
	connect(ui->updateNickButton, SIGNAL(clicked()), SLOT(onUpdateNick()));
	connect(ui->removeNickButton, SIGNAL(clicked()), SLOT(onRemoveNick()));
	connect(ui->moveNickUpButton, SIGNAL(clicked()), SLOT(onMoveNickUp()));
	connect(ui->moveNickDownButton, SIGNAL(clicked()), SLOT(onMoveNickDown()));
	connect(ui->nicksWidget, SIGNAL(currentRowChanged(int)), SLOT(onCurrentNickChanged(int)));
}

IrcNickWizardPage::~IrcNickWizardPage()
{
	delete ui;
}

QStringList IrcNickWizardPage::nicks() const
{
	QStringList list;
	for (int i = 0, count = ui->nicksWidget->count(); i < count; ++i) {
		QListWidgetItem *item = ui->nicksWidget->item(i);
		list << item->text();
	}
	return list;
}

QString IrcNickWizardPage::encoding() const
{
	return ui->encodingBox->currentText();
}

bool IrcNickWizardPage::validatePage()
{
	if (ui->nicksWidget->count()) {
		m_accountWizard->finished();
		return true;
	}
	return false;
}

void IrcNickWizardPage::onAddNick()
{
	QListWidgetItem *item = new QListWidgetItem(ui->nickEdit->text(), ui->nicksWidget);
	ui->nicksWidget->addItem(item);
	ui->nicksWidget->setCurrentItem(item);
}

void IrcNickWizardPage::onRemoveNick()
{
	delete ui->nicksWidget->currentItem();
}

void IrcNickWizardPage::onUpdateNick()
{
	QListWidgetItem *item = ui->nicksWidget->currentItem();
	if (item)
		item->setText(ui->nickEdit->text());
}

void IrcNickWizardPage::onMoveNickUp()
{
	int row = ui->nicksWidget->currentRow();
	moveNick(row, row-1);
}

void IrcNickWizardPage::onMoveNickDown()
{
	int row = ui->nicksWidget->currentRow();
	moveNick(row, row+1);
}

void IrcNickWizardPage::onCurrentNickChanged(int row)
{
	QListWidgetItem *item = ui->nicksWidget->currentItem();
	if (item)
		ui->nickEdit->setText(item->text());
	int nicksCount = ui->nicksWidget->count();
	bool isNickChecked = row >= 0 && row < nicksCount;
	ui->updateNickButton->setEnabled(isNickChecked);
	ui->removeNickButton->setEnabled(isNickChecked);
	ui->moveNickUpButton->setEnabled(row >= 1 && row < nicksCount);
	ui->moveNickDownButton->setEnabled(row >= 0 && row < nicksCount-1);
}

void IrcNickWizardPage::moveNick(int row, int newRow)
{
	Q_ASSERT(row >= 0 && row < ui->nicksWidget->count());
	Q_ASSERT(newRow >= 0 && newRow < ui->nicksWidget->count());
	QListWidgetItem *item = ui->nicksWidget->takeItem(row);
	ui->nicksWidget->insertItem(newRow, item);
	ui->nicksWidget->setCurrentItem(item);
}

IrcAccountCreationWizard::IrcAccountCreationWizard() :
	AccountCreationWizard(IrcProtocol::instance())
{
}

IrcAccountCreationWizard::~IrcAccountCreationWizard()
{
}

QList<QWizardPage *> IrcAccountCreationWizard::createPages(QWidget *parent)
{
	m_mainPage = new IrcAccWizardPage(this, parent);
	m_nicksPage = new IrcNickWizardPage(this, parent);
	QList<QWizardPage *> pages;
	pages << m_mainPage;
	pages << m_nicksPage;
	return pages;
}

void IrcAccountCreationWizard::finished()
{
	IrcAccount *account = IrcProtocol::instance()->getAccount(m_mainPage->networkName(), true);
	// Account config.
	Config cfg = account->config();
	cfg.beginArray("servers");
	int i = 0;
	foreach (const ServerData &server, m_mainPage->servers()) {
		cfg.setArrayIndex(i++);
		cfg.setValue("hostName", server.hostName);
		cfg.setValue<int>("port", server.port);
		cfg.setValue("protectedByPassword", server.protectedByPassword);
		if (server.protectedByPassword && server.savePassword)
			cfg.setValue("password", server.password, Config::Crypted);
	}
	cfg.endArray();
	cfg.setValue("nicks", m_nicksPage->nicks());
	cfg.setValue("codec", m_nicksPage->encoding());
	// Protocol config.
	cfg = IrcProtocol::instance()->config("general");
	QStringList accounts = cfg.value("accounts", QStringList());
	accounts << account->id();
	cfg.setValue("accounts", accounts);
	account->updateSettings();
}

} } // namespace qutim_sdk_0_3::irc

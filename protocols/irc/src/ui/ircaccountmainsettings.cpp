/****************************************************************************
 *  ircaccountmainsettings.cpp
 *
 *  Copyright (c) 2011 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#include "ircaccountmainsettings.h"
#include "ui_addaccountform.h"
#include <qutim/icon.h>
#include <qutim/config.h>

namespace qutim_sdk_0_3 {

namespace irc {

IrcAccountMainSettings::IrcAccountMainSettings(QWidget *parent) :
	QWizardPage(parent), ui(new Ui::AddAccountForm)
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

	registerField("networkName*", ui->networkEdit);
	registerField("servers", ui->serversWidget);
}

IrcAccountMainSettings::~IrcAccountMainSettings()
{
	delete ui;
}

QString IrcAccountMainSettings::networkName() const
{
	return ui->networkEdit->text();
}

void IrcAccountMainSettings::saveToConfig(Config &cfg)
{
	cfg.beginArray("servers");
	int i = 0;
	foreach (const ServerData &server, m_servers) {
		cfg.setArrayIndex(i++);
		cfg.setValue("hostName", server.hostName);
		cfg.setValue<int>("port", server.port);
		cfg.setValue("protectedByPassword", server.protectedByPassword);
		cfg.setValue("ssl", server.ssl);
		if (server.protectedByPassword && server.savePassword)
			cfg.setValue("password", server.password, Config::Crypted);
	}
	cfg.endArray();
}

void IrcAccountMainSettings::reloadSettings(IrcAccount *account)
{
	Config cfg = account->config();
	m_servers.clear();
	ui->serversWidget->clear();
	cfg.beginArray("servers");
	for (int i = 0; i < cfg.arraySize(); ++i) {
		cfg.setArrayIndex(i);
		ServerData server;
		server.hostName = cfg.value("hostName", QString());
		server.ssl = cfg.value("ssl", false);
		server.port = cfg.value("port", server.ssl ? 6667 : 6697);
		server.protectedByPassword = cfg.value("protectedByPassword", false);
		if (server.protectedByPassword)
			server.password = cfg.value("password", QString(), Config::Crypted);
		addServer(server);
	}
	cfg.endArray();
	ui->networkEdit->setEnabled(false);
	ui->networkEdit->setText(account->id());
	emit completeChanged();
}


QWidgetList IrcAccountMainSettings::editableWidgets()
{
	QWidgetList list;
	list << ui->serversWidget;
	return list;
}

void IrcAccountMainSettings::initSettingsWidget(SettingsWidget *widget)
{
	connect(this, SIGNAL(modifiedChanged(bool)), widget, SIGNAL(modifiedChanged(bool)));
	m_widget = widget;
}

void IrcAccountMainSettings::onAddServer()
{
	addServer(currentServer());
	emit completeChanged();
}

void IrcAccountMainSettings::addServer(const ServerData &server)
{
	QListWidgetItem *item = new QListWidgetItem(QString("%1:%2").arg(server.hostName).arg(server.port), ui->serversWidget);
	QListWidgetItem *nullItem = ui->serversWidget->takeItem(m_servers.size());
	ui->serversWidget->insertItem(m_servers.size(), item);
	ui->serversWidget->addItem(nullItem);
	m_servers.push_back(server);
	ui->serversWidget->setCurrentItem(item);
	onCurrentServerChanged(ui->serversWidget->row(item));
}

void IrcAccountMainSettings::onEditServer()
{
	int row = ui->serversWidget->currentRow();
	Q_ASSERT(row >= 0 && row < m_servers.size());
	ServerData server = currentServer();
	m_servers[row] = server;
	ui->serversWidget->currentItem()->setText(QString("%1:%2").arg(server.hostName).arg(server.port));
	if (m_widget && !m_widget->isModified())
		emit modifiedChanged(true);
}

void IrcAccountMainSettings::onRemoveServer()
{
	int row = ui->serversWidget->currentRow();
	Q_ASSERT(row >= 0 && row < m_servers.size());
	m_servers.removeAt(row);
	delete ui->serversWidget->currentItem();
	emit completeChanged();
}

void IrcAccountMainSettings::onMoveUpServer()
{
	int row = ui->serversWidget->currentRow();
	moveServer(row, row-1);
}

void IrcAccountMainSettings::onMoveDownServer()
{
	int row = ui->serversWidget->currentRow();
	moveServer(row, row+1);
}

inline ServerData IrcAccountMainSettings::currentServer()
{
	ServerData server;
	server.hostName = ui->serverEdit->text();
	server.port = ui->portBox->value();
	server.protectedByPassword = ui->protectedByPasswordBox->isChecked();
	server.password = ui->passwordEdit->text();
	server.savePassword = ui->passwordBox->isChecked();
	server.ssl = ui->sslBox->isChecked();
	return server;
}

void IrcAccountMainSettings::moveServer(int row, int newRow)
{
	Q_ASSERT(row >= 0 && row < m_servers.size());
	Q_ASSERT(newRow >= 0 && newRow < m_servers.size());
	QListWidgetItem *item = ui->serversWidget->takeItem(row);
	ui->serversWidget->insertItem(newRow, item);
	ServerData server = m_servers.takeAt(row);
	m_servers.insert(newRow, server);
	ui->serversWidget->setCurrentItem(item);
}

void IrcAccountMainSettings::onCurrentServerChanged(int row)
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
		ui->sslBox->setChecked(server.ssl);
	} else {
		ui->serverEdit->clear();
		ui->portBox->setValue(6667);
		ui->protectedByPasswordBox->setChecked(false);
		ui->passwordEdit->clear();
		ui->passwordBox->setChecked(false);
		ui->sslBox->setChecked(false);
	}
	ui->updateServerButton->setEnabled(isServerChecked);
	ui->removeServerButton->setEnabled(isServerChecked);
	ui->moveUpServerButton->setEnabled(row >= 1 && row < serversCount);
	ui->moveDownServerButton->setEnabled(row >= 0 && row < serversCount-1);
}

void IrcAccountMainSettings::onPasswordProtectionChanged(bool checked)
{
	ui->passwordBox->setEnabled(checked);
	ui->passwordEdit->setEnabled(checked);
	ui->passwordLabel->setEnabled(checked);
}

} } // namespace namespace qutim_sdk_0_3::irc

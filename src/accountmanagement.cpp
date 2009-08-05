/*
    AccountManagement

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "accountmanagement.h"
#include "src/iconmanager.h"

AccountManagement::AccountManagement(QWidget *parent)
    : QWidget(parent)
{
	ui.setupUi(this);
	updateAccountList();
	ui.editButton->setIcon(IconManager::instance().getIcon("edituser"));
	ui.addButton->setIcon(IconManager::instance().getIcon("add"));
	ui.removeButton->setIcon(IconManager::instance().getIcon("remove"));
}

AccountManagement::~AccountManagement()
{

}

void AccountManagement::updateAccountList()
{
	ui.m_account_list->clear();
	PluginSystem &ps = PluginSystem::instance();
	QList<AccountStructure> accounts_list = ps.getAccountsList();
	foreach ( AccountStructure account, accounts_list )
	{
		QListWidgetItem *item = new QListWidgetItem(account.account_name);
		item->setIcon(Icon(account.protocol_name.toLower(), IconInfo::Protocol));
		item->setToolTip(account.protocol_name);
		ui.m_account_list->addItem(item);
	}
	
	if ( ui.m_account_list->count() )
	{
		ui.m_account_list->setCurrentRow(0);
	}
}

void AccountManagement::on_addButton_clicked()
{
	AbstractLayer &al = AbstractLayer::instance();
	al.createNewAccount();
	updateAccountList();
	emit updateAccountComboBoxFromMainSettings();
}

void AccountManagement::on_removeButton_clicked()
{
	if ( ui.m_account_list->count() )
	{
		QListWidgetItem *selected_item = ui.m_account_list->currentItem();
		PluginSystem &ps = PluginSystem::instance();
		ps.removeAccount(selected_item->toolTip(), selected_item->text());
		updateAccountList();
		emit updateAccountComboBoxFromMainSettings();
		AbstractLayer &al = AbstractLayer::instance();
		al.updateStausMenusInTrayMenu();
	}
}

void AccountManagement::on_editButton_clicked()
{
	if ( ui.m_account_list->count() )
	{
		QListWidgetItem *selected_item = ui.m_account_list->currentItem();
		PluginSystem &ps = PluginSystem::instance();
		ps.editAccount(selected_item->toolTip(), selected_item->text());
	}
}

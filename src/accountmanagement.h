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

#ifndef ACCOUNTMANAGEMENT_H
#define ACCOUNTMANAGEMENT_H

#include <QWidget>
#include "ui_accountmanagement.h"
#include "abstractlayer.h"
#include "pluginsystem.h"

class AccountManagement : public QWidget
{
    Q_OBJECT

public:
    AccountManagement(QWidget *parent = 0);
    ~AccountManagement();
private slots:
	void on_addButton_clicked();
	void on_removeButton_clicked();
	void on_editButton_clicked();
signals:
	void updateAccountComboBoxFromMainSettings();
private:
    Ui::AccountManagementClass ui;
    void updateAccountList();
};

#endif // ACCOUNTMANAGEMENT_H

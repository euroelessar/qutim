/*
    loginDialog

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


#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QtGui/QDialog>
#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include "iconmanager.h"

#include "ui_logindialog.h"

    const unsigned char crypter[] = {0x10, 0x67, 0x56, 0x78, 0x85, 0x14, 0x87, 0x11, 0x45, 0x45, 0x45, 0x45, 0x45, 0x45 };

class loginDialog : public QDialog
{
    Q_OBJECT

public:
    loginDialog(QWidget *parent = 0);
    ~loginDialog();
    void loadSettings();
    void loadAccounts();
    void saveSettings();
    void saveAccounts();
    void savePersonalSettings(const QString &);
    void loadPersonalSettings(const QString &);
    void setRunType(bool type) { notStart = type; }

private slots:
	void signInEnable(const QString &);
	void accountChanged(const QString &account) { loadPersonalSettings(account); }
	void accountEdit(const QString &);
	void on_deleteButton_clicked();
	void on_saveBox_stateChanged(int);

signals:
	void addingAccount(const QString &);
	void removingAccount( const QString &);

private:
	QPoint desktopCenter();
	void deleteCurrentAccount(const QString&);
	void removeAccountDir(const QString &);

    Ui::loginDialogClass ui;
    bool notStart;



};

#endif // LOGINDIALOG_H

/*
    requestAuthDialog

    Copyright (c) 2008 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef REQUESTAUTHDIALOG_H
#define REQUESTAUTHDIALOG_H

#include "ui_requestauthdialog.h"

class requestAuthDialog : public QDialog
{
	Q_OBJECT;

public:
	requestAuthDialog(QWidget *parent = 0);
	~requestAuthDialog();
	void setLogin(QString login) { ui.editLogin->setText(login); ui.editPasswd->setFocus(); }
	void setPasswd(QString passwd) { ui.editPasswd->setText(passwd); ui.editPasswd->setFocus(); }
	QString getLogin() { return ui.editLogin->text(); }
	QString getPasswd() { return ui.editPasswd->text(); }
	bool getRemember() { return ui.cbRemember->isChecked(); }
	QString getCode() { return ui.editCaptcha->text(); }
	void setCaptcha(QString);


private:
	Ui::requestAuthDialogClass ui;

};
#endif

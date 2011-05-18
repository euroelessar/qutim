/****************************************************************************
 *  requestauthdialog.h
 *
 *  Copyright (c) 2008-2009 by Alexander Kazarin <boiler@co.ru>
 *                     2010 by Nigmatullin Ruslan <euroelessar@ya.ru>
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

#ifndef REQUESTAUTHDIALOG_H
#define REQUESTAUTHDIALOG_H

#include "ui_requestauthdialog.h"

class YandexNarodRequestAuthDialog : public QDialog
{
	Q_OBJECT;

public:
	YandexNarodRequestAuthDialog(QWidget *parent = 0);
	~YandexNarodRequestAuthDialog();
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

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Alexander Kazarin <boiler@co.ru>
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


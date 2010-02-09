/****************************************************************************
 *  passworddialog.h
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

#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QDialog>

namespace Ui {
	class PasswordForm;
}

namespace Icq {

class IcqAccount;

class PasswordDialog : public QDialog
{
	Q_OBJECT
public:
	PasswordDialog(IcqAccount *account, QWidget *parent = 0);
	virtual ~PasswordDialog();
	QString id() const;
	QString password() const;
	bool isSavePassword() const;
private slots:
	void accepted();
	void validate();
private:
	Ui::PasswordForm *ui;
	QPushButton *m_okButton;
	IcqAccount *m_account;
};

} // namespace Icq

#endif // PASSWORDDIALOG_H

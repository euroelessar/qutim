/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Evgeniy Degtyarev <degtep@gmail.com>
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

#ifndef PASSWORDDIALOGWRAPPER_H
#define PASSWORDDIALOGWRAPPER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include "quickpassworddialog.h"
namespace MeegoIntegration
{

class PasswordDialogWrapper : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(bool rememberPassword READ rememberPassword WRITE setRememberPassword NOTIFY rememberPasswordChanged)
	Q_PROPERTY(QString passwordText READ passwordText WRITE setPasswordText NOTIFY passwordTextChanged)
public:
	PasswordDialogWrapper();
	~PasswordDialogWrapper();
	QString title() const;
	QString passwordText() const;
	bool rememberPassword();
	void setTitle(const QString &);
	void setRememberPassword(bool);
	void setPasswordText(const QString &);
	Q_INVOKABLE void accept();
	Q_INVOKABLE void cancel();
	static void init();
	static void showDialog(QString title, QuickPasswordDialog * passDialog);

signals:
	void rememberPasswordChanged();
	void titleChanged();
	void passwordTextChanged();
	void shown();

private:
	QString m_dialogTitle;
	QString m_passwordText;
	bool m_rememberPassword;
	static QuickPasswordDialog *m_currentDialog;

};
}

#endif /* PASSWORDDIALOGMANAGER_H */

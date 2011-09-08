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

#ifndef QUICKPASSWORDDIALOGMANAGER_H
#define QUICKPASSWORDDIALOGMANAGER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include "quickpassworddialog.h"
namespace MeegoIntegration
{

class QuickPasswordDialogManager : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
	Q_PROPERTY(bool rememberPassword READ rememberPassword WRITE setRememberPassword NOTIFY rememberPasswordChanged)
	Q_PROPERTY(QString passwordText READ passwordText WRITE setPasswordText NOTIFY passwordTextChanged)
public:
	QuickPasswordDialogManager();
	~QuickPasswordDialogManager();
	static void init();
	QString title();
	QString passwordText();
	bool rememberPassword();
	void setTitle(QString title);
	void setRememberPassword(bool rememberPassword);
	void setPasswordText(QString passwordText);
	Q_INVOKABLE void accept();
	Q_INVOKABLE void cancel();
	static void showDialog(QString title, QuickPasswordDialog * passDialog);

signals:
	void rememberPasswordChanged(bool rememberPassword);
	void titleChanged(const QString &titleText);
	void passwordTextChanged(const QString &passwordText);
	void shown();

private:
	QString m_dialogTitle;
	QString m_passwordText;
	bool m_rememberPassword;
	static QList<QuickPasswordDialogManager*> m_managers;
	static QuickPasswordDialog *m_currentDialog;

};
}

#endif /* PASSWORDDIALOGMANAGER_H */

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

#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include <QtCore/QObject>
#include <QtCore/QStringList>


class PasswordDialog : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString title READ title CONSTANT)
	Q_PROPERTY(bool rememberPassword READ rememberPassword WRITE setRememberPassword NOTIFY rememberPasswordChanged)
public:
	PasswordDialog();
	~PasswordDialog();
	
	QString title() const;
	bool rememberPassword() const;
	void setRememberPassword(bool rememberPassword);
	Q_INVOKABLE void accept();
	Q_INVOKABLE void reject();
signals:
	void rememberPasswordChanged(bool rememberPassword);
private:
	QString dialogTitle;
	bool m_rememberPassword;

};

#endif /* PASSWORDDIALOG_H*/

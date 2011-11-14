/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef MD5LOGIN_H
#define MD5LOGIN_H

#include "oscarconnection.h"
#include <QHostInfo>

namespace qutim_sdk_0_3 {

namespace oscar {

class Md5Login: public AbstractConnection
{
	Q_OBJECT
public:
	Md5Login(const QString &password, IcqAccount *account);
	virtual ~Md5Login();
public slots:
	void login();
protected:
	virtual void processNewConnection();
	virtual void processCloseConnection();
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
private slots:
	void hostFound(const QHostInfo &host);
private:
	void setLoginData(const QString &addr, quint16 port, const QByteArray &cookie);
	QString m_host;
	QString m_addr;
	quint16 m_port;
	QByteArray m_cookie;
	OscarConnection *m_conn;
	QString m_password;
	int m_hostReqId;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // MD5LOGIN_H


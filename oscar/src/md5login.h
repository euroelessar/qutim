/****************************************************************************
 *  md5login.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *                        Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef MD5LOGIN_H
#define MD5LOGIN_H

#include "oscarconnection.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class Md5Login: public AbstractConnection
{
	Q_OBJECT
public:
	Md5Login(const QString &password, OscarConnection *conn);
	virtual ~Md5Login();
	void login();
protected:
	virtual void processNewConnection();
	virtual void processCloseConnection();
	void setError(ConnectionError error) { AbstractConnection::setError(error); };
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
private:
	void setLoginData(const QString &addr, quint16 port, const QByteArray &cookie);
	QString m_addr;
	quint16 m_port;
	QByteArray m_cookie;
	OscarConnection *m_conn;
	QString m_password;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // MD5LOGIN_H

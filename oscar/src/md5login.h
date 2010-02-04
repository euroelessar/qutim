/****************************************************************************
 *  md5login.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

namespace Icq
{

class Md5LoginNegotiation: public SNACHandler
{
	Q_OBJECT
public:
	Md5LoginNegotiation(OscarConnection *conn, QObject *parent = 0);
	virtual void handleSNAC(AbstractConnection *conn, const SNAC &snac);
	void setConnection(OscarConnection *conn) { m_conn = conn; }
private:
	OscarConnection *m_conn;
};

class Md5Login: public AbstractConnection
{
	Q_OBJECT
public:
	Md5Login(OscarConnection *conn);
	~Md5Login();
	void login();
protected:
	void processNewConnection();
	void processCloseConnection();
	void setError(ConnectionError error) { AbstractConnection::setError(error); };
private:
	void setLoginData(const QString &addr, quint16 port, const QByteArray &cookie);
	friend class Md5LoginNegotiation;
	QString m_addr;
	quint16 m_port;
	QByteArray m_cookie;
	OscarConnection *m_conn;
};

} // namespace Icq

#endif // MD5LOGIN_H

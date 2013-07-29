/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef OSCARAUTH_H
#define OSCARAUTH_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QObjectCleanupHandler>
#include <qutim/networkproxy.h>
#include "connection.h"

class QUrl;

namespace qutim_sdk_0_3 {

namespace oscar {

class IcqAccount;

class OscarAuth : public QObject
{
    Q_OBJECT
public:
	enum State
	{
		Invalid,
		AtPasswordRequest,
		AtLogin,
		AtSessionStart,
		AtError
	};
	
    explicit OscarAuth(IcqAccount *account);
    ~OscarAuth();
	
	State state() const { return m_state; }
	QString errorString() const { return m_errorString; }
	
public slots:
	void setProxy(const QNetworkProxy &proxy);
	void login();

protected:
	void clientLogin(bool longTerm);
	void startSession(const QByteArray &token, const QByteArray &sessionKey);
	
signals:
	void stateChanged(qutim_sdk_0_3::oscar::OscarAuth::State);
	void error(qutim_sdk_0_3::oscar::AbstractConnection::ConnectionError error);
	
private slots:
	void onPasswordDialogFinished(int result);
	void onClientLoginFinished();
	void onStartSessionFinished();
    void onSslErrors(const QList<QSslError> &errors);

private:
	QPair<QLatin1String, QLatin1String> getDistInfo() const;
	QString getDistId() const;
	QString getClientName() const;
	QString generateLanguage();
	QByteArray generateSignature(const QByteArray &method, const QByteArray &sessionSecret, const QUrl &url, const QUrlQuery &query);
	
	IcqAccount *m_account;
	State m_state;
	QNetworkAccessManager m_manager;
	QString m_password;
	QString m_errorString;
	QObjectCleanupHandler m_cleanupHandler;
};

} } // namespace qutim_sdk_0_3::oscar

#endif // OSCARAUTH_H


/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef VCONNECTION_H
#define VCONNECTION_H
#include "vkontakte_global.h"
#include <QNetworkAccessManager>
#include "vreply.h"

class VRoster;
class VMessages;
namespace qutim_sdk_0_3 {
	class Config;
}

class VRequest;
class VAccount;
class VConnectionPrivate;
class LIBVKONTAKTE_EXPORT VConnection : public QNetworkAccessManager
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VConnection)
public:
	explicit VConnection(VAccount *account, QObject* parent = 0);
	VConnectionState connectionState() const;
	virtual ~VConnection();
	QNetworkReply *get(const QString &method, const QVariantMap &args = QVariantMap());
	VReply *request(const QString &method, const QVariantMap &args = QVariantMap());
	VReply *request(const QString &method, const QString &name, const QVariant &value);
	Config config();
	Config config(const QString &name);
	VAccount *account() const;
	VMessages *messages() const;
	VRoster *roster() const;
public slots:
	void connectToHost();
	void disconnectFromHost(bool force = false);
	void saveSettings();
	void loadSettings();
	void onLoadFinished(bool);
signals:
	void connectionStateChanged(VConnectionState state);
protected:
	void setConnectionState(VConnectionState state);
private:
	QScopedPointer<VConnectionPrivate> d_ptr;

	Q_PRIVATE_SLOT(d_func(), void _q_on_error(QNetworkReply::NetworkError error))
	Q_PRIVATE_SLOT(d_func(), void _q_on_reply_finished())
	Q_PRIVATE_SLOT(d_func(), void _q_on_webview_destroyed())
};

inline VReply *VConnection::request(const QString &method, const QVariantMap &args)
{
	return new VReply(get(method, args));
}

inline VReply *VConnection::request(const QString &method, const QString &name, const QVariant &value)
{
	QVariantMap args;
	args.insert(name, value);
	return new VReply(get(method, args));
}

#endif // VCONNECTION_H


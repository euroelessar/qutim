/****************************************************************************
 *  vconnection.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#ifndef VCONNECTION_H
#define VCONNECTION_H
#include "vkontakte_global.h"
#include <QNetworkAccessManager>

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
	QNetworkReply *get(VRequest &request);
	QNetworkReply *get(const QString &method, const QVariantMap &args = QVariantMap());
	Config config();
	Config config(const QString &name);
	VAccount *account() const;
	VMessages *messages() const;
	VRoster *roster() const;
public slots:
	void connectToHost(const QString &passwd);
	void disconnectFromHost(bool force = false);
	void loadSettings();
	void onLoadFinished(bool);
signals:
	void connectionStateChanged(VConnectionState state);
protected:
	void setConnectionState(VConnectionState state);
private:
	QScopedPointer<VConnectionPrivate> d_ptr;
};

#endif // VCONNECTION_H

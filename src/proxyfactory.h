/*****************************************************************************
	ProxyFactory

	Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#include <QUrl>

#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))

#ifndef PROXYFACTORY_H
#define PROXYFACTORY_H

#include <QNetworkProxyFactory>

class ProxyFactory : public QObject, public QNetworkProxyFactory
{
	Q_OBJECT
public:
    ProxyFactory();
	virtual ~ProxyFactory();
	void loadSettings();
	virtual QList<QNetworkProxy> queryProxy(const QNetworkProxyQuery &query = QNetworkProxyQuery());
protected:
	QNetworkProxy m_proxy;
};

#endif // PROXYFACTORY_H

#endif

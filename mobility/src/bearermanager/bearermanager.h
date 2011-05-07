/****************************************************************************
 *  bearermanager.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef BEARERMANAGER_H
#define BEARERMANAGER_H

#include <QObject>

class QNetworkConfigurationManager;
class BearerManager : public QObject
{
    Q_OBJECT
	Q_CLASSINFO("Service", "Bearer")
	Q_CLASSINFO("Uses", "SettingsLayer")
public:
    explicit BearerManager(QObject *parent = 0);
signals:
	void onlineStateChanged(bool isOnline);
private slots:
	void onOnlineStatusChanged(bool isOnline);
private:
	QNetworkConfigurationManager *m_confManager;
};

#endif // BEARERMANAGER_H

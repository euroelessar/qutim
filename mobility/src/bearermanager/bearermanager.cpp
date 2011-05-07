/****************************************************************************
 *  bearermanager.cpp
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

#include "bearermanager.h"
#include <QNetworkConfigurationManager>

BearerManager::BearerManager(QObject *parent) :
	QObject(parent),
	m_confManager(new QNetworkConfigurationManager(this))
{
	Q_UNUSED(QT_TRANSLATE_NOOP("Service", "BearerManager"));

	connect(m_confManager, SIGNAL(onlineStateChanged(bool)), SLOT(onOnlineStatusChanged(bool)));
	emit onlineStateChanged(m_confManager->isOnline());
}

void BearerManager::onOnlineStatusChanged(bool isOnline)
{
	//TODO handle status
	emit onlineStateChanged(isOnline);
}

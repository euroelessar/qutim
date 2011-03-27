/****************************************************************************
 *  vrequest.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef VREQUEST_H
#define VREQUEST_H

#include <QNetworkRequest>
#include "vkontakte_global.h"

class VRequest : public QNetworkRequest
{
public:
	VRequest(const QUrl& url = QUrl());
};

#endif // VREQUEST_H

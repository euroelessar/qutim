/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef VREPLY_H
#define VREPLY_H

#include <QNetworkReply>

class VReply : public QObject
{
	Q_OBJECT
public:
	enum ErrorCode
	{
		CaptchaNeeded = 14
	};
    explicit VReply(QNetworkReply *parent);
signals:
	void resultReady(const QVariant &var, bool error);
private slots:
	void onRequestFinished();
};

#endif // VREPLY_H

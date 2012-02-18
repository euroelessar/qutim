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


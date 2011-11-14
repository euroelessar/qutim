/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ivan Vizir <define-true-false@yandex.com>
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

#include "winint.h"
#include <QTcpServer>
#include <QMap>

class WININT_EXPORTS CmdServer : public QObject
{
	Q_OBJECT

public:
	void registerHandler(const QString &prefix, QObject *receiver);
	static CmdServer *instance();

signals:

public slots:
	void onNewConnection();
	void onRcvDestroyed(QObject* obj);

private:
	CmdServer();
	QMap<QString, QObject*> handlers_;
	QTcpServer listenner_;
};


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

#ifndef STATISTICSHELPER_P_H
#define STATISTICSHELPER_P_H

#include "libqutim_global.h"
#include <QVariantMap>
#include <QNetworkReply>

namespace qutim_sdk_0_3 {

class Config;
class StatisticsHelperPrivate;

class LIBQUTIM_EXPORT StatisticsHelper : public QObject
{
    Q_OBJECT
	Q_DECLARE_PRIVATE(StatisticsHelper)
	Q_ENUMS(Action)
	Q_PROPERTY(QString infoHtml READ infoHtml CONSTANT)
	Q_PROPERTY(Action action READ action CONSTANT)
public:
	enum Action {
		NeedToAskInit,
		NeedToAskUpdate,
		NothingToAsk,
		DeniedToSend
	};

    explicit StatisticsHelper(QObject *parent = 0);
	~StatisticsHelper();
	
	QString infoHtml() const;
	Action action() const;
	Q_INVOKABLE void setDecisition(bool denied, bool automatic);
	
private slots:
	void _q_on_finished();

private:
//	Q_PRIVATE_SLOT(d_func(), void _q_on_finished())
	QScopedPointer<StatisticsHelperPrivate> d_ptr;
};

}

#endif // STATISTICSHELPER_P_H


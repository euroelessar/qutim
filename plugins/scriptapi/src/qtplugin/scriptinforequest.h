/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
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

#ifndef SCRIPTINFOREQUEST_H
#define SCRIPTINFOREQUEST_H

#include <qutim/inforequest.h>
#include <QScriptValue>

namespace qutim_sdk_0_3
{
class ScriptInfoRequest : public QObject
{
    Q_OBJECT
public:
	explicit ScriptInfoRequest(const QScriptValue &func, const QScriptValue &error,
							   InfoRequest *parent);

signals:

public slots:
	void onStateChanged(qutim_sdk_0_3::InfoRequest::State);

private:
	void handleError(const char *name, const QString &text);
	
private:
	QScriptValue m_func;
	QScriptValue m_error;
};
}

#endif // SCRIPTINFOREQUEST_H

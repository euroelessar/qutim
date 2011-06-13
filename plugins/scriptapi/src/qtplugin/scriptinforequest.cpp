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

#include "scriptinforequest.h"
#include <QScriptEngine>
#include <QDebug>

namespace qutim_sdk_0_3
{
ScriptInfoRequest::ScriptInfoRequest(const QScriptValue &func, const QScriptValue &error, InfoRequest *parent)
    : QObject(parent), m_func(func), m_error(error)
{
	if (!parent) {
		qDebug() << Q_FUNC_INFO;
		deleteLater();
		return;
	}
	qDebug() << Q_FUNC_INFO;
	connect(parent, SIGNAL(stateChanged(qutim_sdk_0_3::InfoRequest::State)),
	        SLOT(onStateChanged(qutim_sdk_0_3::InfoRequest::State)));
	connect(this, SIGNAL(destroyed()), parent, SLOT(deleteLater()));
	onStateChanged(parent->state());
}

void ScriptInfoRequest::onStateChanged(qutim_sdk_0_3::InfoRequest::State state)
{
	qDebug() << Q_FUNC_INFO << state;
	if (state == InfoRequest::Request)
		return;
	InfoRequest *request = qobject_cast<InfoRequest*>(parent());
	Q_ASSERT(request);
	QScriptEngine *engine = m_func.engine();
	if (state == InfoRequest::Cache)
		request->deleteLater();
	if (state == InfoRequest::Cancel) {
		request->deleteLater();
		qDebug() << Q_FUNC_INFO << m_error.isFunction();
		if (m_error.isFunction()) {
			QScriptValue error = engine->newObject();
			error.setProperty(QLatin1String("name"), QLatin1String("Canceled"));
			error.setProperty(QLatin1String("text"), QLatin1String("Request was canceled"));
			QList<QScriptValue> args;
			args << error;
			m_error.call(m_error, args);
		}
		deleteLater();
		return;
	}
	DataItem item = request->item();
	QList<QScriptValue> args;
	args << engine->toScriptValue(item);
	qDebug() << Q_FUNC_INFO;
	Q_ASSERT(m_func.isFunction());
	m_func.call(m_func, args);
	deleteLater();
}
}

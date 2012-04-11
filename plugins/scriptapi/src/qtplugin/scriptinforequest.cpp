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

#include "scriptinforequest.h"
#include <QScriptEngine>
#include <qutim/debug.h>

namespace qutim_sdk_0_3
{
ScriptInfoRequest::ScriptInfoRequest(const QScriptValue &func, const QScriptValue &error,
									 InfoRequest *parent)
    : QObject(parent), m_func(func), m_error(error)
{
	if (!parent) {
		debug() << Q_FUNC_INFO;
		deleteLater();
		return;
	}
	debug() << Q_FUNC_INFO;
	connect(parent, SIGNAL(stateChanged(qutim_sdk_0_3::InfoRequest::State)),
			SLOT(onStateChanged(qutim_sdk_0_3::InfoRequest::State)));
	connect(this, SIGNAL(destroyed()), parent, SLOT(deleteLater()));

	InfoRequest::State state = parent->state();
	if (state == InfoRequest::Initialized)
		parent->requestData();
	else if (state == InfoRequest::LoadedFromCache)
		onStateChanged(state);
}

void ScriptInfoRequest::onStateChanged(qutim_sdk_0_3::InfoRequest::State state)
{
	debug() << Q_FUNC_INFO << state;
	if (state == InfoRequest::Requesting)
		return;
	InfoRequest *request = qobject_cast<InfoRequest*>(parent());
	Q_ASSERT(request);
	if (state == InfoRequest::LoadedFromCache) {
		request->deleteLater();
	} else if (state == InfoRequest::Canceled) {
		handleError("Canceled", "Request was canceled");
		return;
	} else if (state == InfoRequest::Error) {
		handleError("Error", request->errorString().toString());
		return;
	}
	DataItem item = request->dataItem();
	QList<QScriptValue> args;
	args << m_func.engine()->toScriptValue(item);
	debug() << Q_FUNC_INFO;
	Q_ASSERT(m_func.isFunction());
	m_func.call(m_func, args);
	deleteLater();
}

void ScriptInfoRequest::handleError(const char *name, const QString &text)
{
	InfoRequest *request = qobject_cast<InfoRequest*>(parent());
	Q_ASSERT(request);
	request->deleteLater();
	debug() << Q_FUNC_INFO << m_error.isFunction();
	if (m_error.isFunction()) {
		QScriptValue error = m_func.engine()->newObject();
		error.setProperty(QLatin1String("name"), name);
		error.setProperty(QLatin1String("text"), text);
		QList<QScriptValue> args;
		args << error;
		m_error.call(m_error, args);
	}
	deleteLater();
}

}


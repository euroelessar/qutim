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

#ifndef SCRIPTENGINEDATA_H
#define SCRIPTENGINEDATA_H

#include <QSharedPointer>
#include <QMetaType>

class QScriptEngine;

namespace qutim_sdk_0_3
{
class ScriptMessage;
class ScriptServices;
class ScriptMessageHandler;

class ScriptEngineData
{
	Q_DISABLE_COPY(ScriptEngineData)
public:
	typedef QSharedPointer<ScriptEngineData> Ptr;
	
    ScriptEngineData();
	~ScriptEngineData();

	static ScriptEngineData *data(QScriptEngine *engine);
	
	QScriptEngine *engine;
	ScriptMessage *message;
	ScriptServices *services;
	ScriptMessageHandler *messageHandler;
};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::ScriptEngineData::Ptr)

#endif // SCRIPTENGINEDATA_H

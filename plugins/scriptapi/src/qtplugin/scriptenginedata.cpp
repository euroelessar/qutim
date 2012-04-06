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

#include "scriptenginedata.h"
#include "scriptmessage.h"
#include "scriptmessagehandler.h"
#include "scriptservices.h"
#include "scriptdataitem.h"
#include <QScriptEngine>

namespace qutim_sdk_0_3
{
typedef QMap<QScriptEngine*, ScriptEngineData*> DataMap;
Q_GLOBAL_STATIC(DataMap, dataMap)

ScriptEngineData::ScriptEngineData()
{
}

ScriptEngineData::~ScriptEngineData()
{
	delete message;
	delete dataItem;
	delete messageHandler;
	delete services;
	if (dataMap())
		dataMap()->remove(engine);
}

ScriptEngineData *ScriptEngineData::data(QScriptEngine *engine)
{
	ScriptEngineData * &engineData = (*dataMap())[engine];
	if (!engineData) {
		ScriptEngineData::Ptr ptr = ScriptEngineData::Ptr::create();
		engineData = ptr.data();
		qMemSet(ptr.data(), 0, sizeof(*ptr.data()));
		engineData->engine = engine;
		engine->setProperty("scriptEngineData", qVariantFromValue(ptr));
	}
	return engineData;
}
}


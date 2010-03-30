#ifndef SCRIPTTOOLS_H
#define SCRIPTTOOLS_H

#include "libqutim_global.h"
#include <QScriptEngine>

class QDeclarativeEngine;

namespace qutim_sdk_0_3
{
	class ScriptTools
	{
	public:
		static void prepareEngine(QScriptEngine *engine);
		static void prepareEngine(QDeclarativeEngine *engine);
	private:
		ScriptTools();
		~ScriptTools();
	};
}

#endif // SCRIPTTOOLS_H

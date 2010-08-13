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

#ifndef SCRIPTPLUGINWRAPPER_H
#define SCRIPTPLUGINWRAPPER_H

#include <qutim/plugin.h>

class ScriptEngine;
class QScriptValue;

class ScriptPluginWrapper : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
public:
    explicit ScriptPluginWrapper(const QString &name);
	virtual ~ScriptPluginWrapper();
	
	virtual void init();
	virtual bool load();
	virtual bool unload();
private slots:
	void onException(const QScriptValue &exception);
private:
	QString m_name;
	ScriptEngine *m_engine;
	bool m_apiInited;
};

#endif // SCRIPTPLUGINWRAPPER_H

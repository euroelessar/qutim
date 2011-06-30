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

#ifndef DBUSPLUGIN_H
#define DBUSPLUGIN_H

#include <qutim/plugin.h>
#include <qutim/messagehandler.h>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScriptEngine>

using namespace qutim_sdk_0_3;

class ScriptPlugin;

class ScriptMessageHandler : public qutim_sdk_0_3::MessageHandler
{
public:
	ScriptMessageHandler(ScriptPlugin *parent);
	virtual Result doHandle(qutim_sdk_0_3::Message &message, QString *reason);
	
	void openContext(ChatUnit *unit);
	void closeContext();
	void handleException();
private:
	QScriptEngine m_engine;
};

class ScriptPlugin : public Plugin, public qutim_sdk_0_3::PluginFactory
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::PluginFactory)
public:
    explicit ScriptPlugin();
	virtual void init();
	virtual bool load();
	virtual QList<Plugin*> loadPlugins();
	virtual bool unload();
	
public slots:
	void onException(const QScriptValue &exception);
	
private:
	QScopedPointer<ScriptMessageHandler> m_handler;
};

#endif // DBUSPLUGIN_H

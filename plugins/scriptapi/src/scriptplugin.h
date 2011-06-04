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
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScriptEngine>

using namespace qutim_sdk_0_3;

class ScriptHelperWidget : public QWidget
{
    Q_OBJECT
public:
    ScriptHelperWidget();
    virtual ~ScriptHelperWidget() {}
	
public slots:
	void onButtonClicked();
	
private:
	QPlainTextEdit *m_textEdit;
	QScriptEngine m_engine;
};

class ScriptPlugin : public Plugin, public PluginFactory
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::PluginFactory)
public:
    explicit ScriptPlugin();
	virtual void init();
	virtual bool load();
	virtual QList<Plugin*> loadPlugins();
	virtual bool unload();
};

#endif // DBUSPLUGIN_H

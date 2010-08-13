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

#ifndef SCRIPTENGINE_H
#define SCRIPTENGINE_H

#include <QScriptEngine>

class ScriptEngine : public QScriptEngine
{
	Q_OBJECT
public:
    explicit ScriptEngine(const QString &name, QObject *parent = 0);
	void initApi();
	inline QString name() const { return m_name; }
private:
	QString m_name;
};

#endif // SCRIPTENGINE_H

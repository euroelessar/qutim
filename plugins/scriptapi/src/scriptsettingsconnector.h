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

#ifndef SCRIPTSETTINGSCONNECTOR_H
#define SCRIPTSETTINGSCONNECTOR_H

#include <QObject>

class ScriptSettingsConnector : public QObject
{
	Q_OBJECT
public:
	ScriptSettingsConnector(QObject *parent);
	virtual ~ScriptSettingsConnector();
signals:
	void saved();
public slots:
	void onSaved() { emit saved(); }
};

#endif // SCRIPTSETTINGSCONNECTOR_H

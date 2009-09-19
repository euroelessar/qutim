/*
    W7 integration plugin

    Copyright (c) 2009 by Belov Nikita <null@deltaz.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef W7IWTASKBAR_H
#define W7IWTASKBAR_H

#include <shobjidl.h>
#include "wIcon.h"

class w7itaskbar
{
public:
	w7itaskbar();
	~w7itaskbar();

	bool init();
	void setHWND( HWND id, int type );
	void tabAlert( TreeModelItem *item, bool state );

private:
	void setOverlayIcon( HWND wid, QString icon = QString() );

	ITaskbarList3 *m_taskBarList;

	QHash<int, HWND> windowsId;
	QHash<int, int> alerts;

	QList<QString> activeAlerts;

	int tabWindowType,
			cAlertsCommonTab,
			cAlertsChatsTab,
			cAlertsConfsTab;
};
#endif

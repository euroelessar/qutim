/*
    AbstractContextLayer

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef ABSTRACTCONTEXTLAYER_H_
#define ABSTRACTCONTEXTLAYER_H_

#include "pluginsystem.h"

class AbstractContextLayer : public QObject, public EventHandler
{
	Q_OBJECT
public:
	static AbstractContextLayer &instance();
	void itemContextMenu(const TreeModelItem &item, const QPoint &menu_point);
	void conferenceItemContextMenu(const QString &protocol_name, const QString &conference_name,
			const QString &account_name, const QString &nickname, const QPoint &menu_point);
	void registerContactMenuAction(QAction *plugin_action);
	void createActions();
	void processEvent(Event &event);
};

#endif /*ABSTRACTCONTEXTLAYER_H_*/

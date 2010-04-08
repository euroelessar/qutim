/****************************************************************************
 *  historymanagerplugin.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef historymanagerplugin_H
#define historymanagerplugin_H

#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

class HistoryManagerPlugin : public Plugin
{
	Q_OBJECT
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
public slots:
	void createWidget();
private:
	QPointer<QWidget> m_widget;
};

#endif

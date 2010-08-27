/****************************************************************************
 *  quetzalplugin.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef PURPLEPROTOCOLPLUGIN_H
#define PURPLEPROTOCOLPLUGIN_H

#include <qutim/protocol.h>
#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

class QuetzalPlugin : public Plugin
{
    Q_OBJECT
	Q_CLASSINFO("DebugName", "quetzal")
public:
	virtual void init();
	virtual bool load() { return true; }
	virtual bool unload() { return false; }
	void clearTemporaryDir();
private slots:
	void onFinished(void *data);
private:
	void initLibPurple();
	QString m_tmpDir;
};

#endif // PURPLEPROTOCOLPLUGIN_H

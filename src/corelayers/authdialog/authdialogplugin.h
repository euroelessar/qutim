/****************************************************************************
 *  authdialogplugin.h
 *
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef AUTHDIALOGPLUGIN_H
#define AUTHDIALOGPLUGIN_H

#include <QObject>
#include <qutim/plugin.h>

namespace Core {

class AuthDialogPlugin : public qutim_sdk_0_3::Plugin
{
    Q_OBJECT
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
};

} // namespace Core

#endif // AUTHDIALOGPLUGIN_H

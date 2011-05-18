/****************************************************************************
 *  mrimplugin.h
 *
 *  Copyright (c) 2009 by Rusanov Peter <peter.rusanov@gmail.com>
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

#ifndef MRIMPLUGIN_H
#define MRIMPLUGIN_H

#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

class MrimPlugin : public Plugin
{
    Q_OBJECT
    Q_CLASSINFO("DebugName","MRIM")
public:
    MrimPlugin();
    void init();
    bool load();
    bool unload();
};

#endif // MRIMPLUGIN_H

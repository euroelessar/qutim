/****************************************************************************
 *  actionbox_p.h
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

#ifndef ACTIONBOX_P_H
#define ACTIONBOX_P_H
#include <QObject>

namespace qutim_sdk_0_3
{

class ActionBox;
class ActionBoxModule;
class ActionBoxPrivate
{
	Q_DECLARE_PUBLIC(ActionBox)
public:
	ActionBoxPrivate() : q_ptr(0),module(0) {}
	ActionBox *q_ptr;
	ActionBoxModule *module;
};

}
#endif // ACTIONBOX_P_H

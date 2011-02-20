/****************************************************************************
 *  contactlist.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef CONTACTLIST_H
#define CONTACTLIST_H

#include <QObject>
#include "menucontroller.h"

namespace qutim_sdk_0_3 {

//Dummy
//TODO rename to MainMenu
class LIBQUTIM_EXPORT ContactList : public MenuController
{
    Q_OBJECT
	Q_CLASSINFO("Service", "ContactList")
public:
};



} // namespace qutim_sdk_0_3

#endif // CONTACTLIST_H

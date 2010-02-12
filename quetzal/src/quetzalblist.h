/****************************************************************************
 *  quetzalblist.h
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

#ifndef QUETZALBLIST_H
#define QUETZALBLIST_H

#include <purple.h>

extern PurpleBlistUiOps quetzal_blist_uiops;

extern GList *quetzal_blist_get_chats(PurpleAccount *acc);

Q_DECLARE_METATYPE(PurpleChat*)

#endif // QUETZALBLIST_H

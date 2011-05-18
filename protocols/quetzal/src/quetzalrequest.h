/****************************************************************************
 *  quetzalrequest.h
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

#ifndef QUETZALREQUEST_H
#define QUETZALREQUEST_H

#include <QString>
#include <purple.h>

extern PurpleRequestUiOps quetzal_request_uiops;

extern QString quetzal_create_label(const char *primary, const char *secondary);

#endif // QUETZALREQUEST_H

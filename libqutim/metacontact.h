/****************************************************************************
 *  metacontact.h
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

#ifndef METACONTACT_H
#define METACONTACT_H

#include "contact.h"

namespace qutim_sdk_0_3
{
	class LIBQUTIM_EXPORT MetaContact : public Contact
	{
		Q_OBJECT
	public:
		MetaContact();
	};
}

#endif // METACONTACT_H

/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef VARIANTHOOK_P_H
#define VARIANTHOOK_P_H

#include <QVariant>
#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	class VariantHook : public QVariant
	{
	public:
		static void init();
		
	private:
		static const QVariant::Handler handler;
		static const QVariant::Handler *lastHandler;
	};
}

#endif // VARIANTHOOK_P_H

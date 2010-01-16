/****************************************************************************
 *  profilecreatorpage.h
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

#ifndef PROFILECREATORPAGE_H
#define PROFILECREATORPAGE_H

#include "abstractwizardpage.h"

namespace qutim_sdk_0_3
{
	class LIBQUTIM_EXPORT ProfileCreatorPage : public AbstractWizardPage
	{
		Q_OBJECT
	protected:
		ProfileCreatorPage();
		virtual ~ProfileCreatorPage();
	};
}

#endif // PROFILECREATORPAGE_H

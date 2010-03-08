/****************************************************************************
 *  profilecreatorpage.cpp
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

#include "profilecreatorpage.h"
#include <QVariant>

namespace qutim_sdk_0_3
{
	ProfileCreatorPage::ProfileCreatorPage()
	{
	}

	ProfileCreatorPage::~ProfileCreatorPage()
	{
	}

	double ProfileCreatorPage::priority() const
	{
		return 0.0;
	}

	ExtensionInfo ProfileCreatorPage::info() const
	{
		return property("protocolinfo").value<ExtensionInfo>();
	}
}

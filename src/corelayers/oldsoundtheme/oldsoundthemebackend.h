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

#ifndef OLDSOUNDTHEMEBACKEND_H
#define OLDSOUNDTHEMEBACKEND_H

#include "oldsoundthemeprovider.h"

namespace Core
{
class OldSoundThemeBackend : public SoundThemeBackend
{
	Q_OBJECT
public:
	virtual QStringList themeList();
	virtual SoundThemeProvider *loadTheme(const QString &name);
};
}

#endif // OLDSOUNDTHEMEBACKEND_H

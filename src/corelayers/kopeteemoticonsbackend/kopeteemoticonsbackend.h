/****************************************************************************
 *  kopeteemoticonsbackend.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#ifndef KOPETEEMOTICONSBACKEND_H
#define KOPETEEMOTICONSBACKEND_H
#include <qutim/emoticons.h>

using namespace qutim_sdk_0_3;

class KopeteEmoticonsBackend : public EmoticonsBackend
{
	Q_OBJECT
public:
    virtual EmoticonsProvider* loadTheme(const QString& name);
    virtual QStringList themeList();	
    virtual ~KopeteEmoticonsBackend();
};

#endif // KOPETEEMOTICONSBACKEND_H

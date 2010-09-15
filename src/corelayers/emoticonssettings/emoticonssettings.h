/****************************************************************************
 *  emoticonssettings.h
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

#ifndef EMOTICONSSETTINGS_H
#define EMOTICONSSETTINGS_H

#include <QObject>
#include "qutim/startupmodule.h"

namespace Core {

	class EmoticonsSettings : public QObject
	{
		Q_OBJECT
	public:
		EmoticonsSettings();
	};

}

#endif // EMOTICONSSETTINGS_H

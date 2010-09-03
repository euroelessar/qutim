/****************************************************************************
 * weather.h
 *
 *  Copyright (c) 2010 by Belov Nikita <null@deltaz.org>
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

#ifndef WEATHER_H
#define WEATHER_H

#include <qutim/plugin.h>

#include "wprotocol.h"

using namespace qutim_sdk_0_3;

class WeatherPlugin : public Plugin
{
	Q_OBJECT

public:
	WeatherPlugin();
	virtual void init();
	virtual bool load();
	virtual bool unload();
private:
	QPointer<WProtocol> m_protocol;
};

#endif // WEATHER_H

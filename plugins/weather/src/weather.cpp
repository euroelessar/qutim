/****************************************************************************
 * weather.cpp
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

#include <qutim/icon.h>

#include "weather.h"

WeatherPlugin::WeatherPlugin() : m_protocol(0)
{
}

void WeatherPlugin::init()
{
	addAuthor( QT_TRANSLATE_NOOP( "Author", "Nikita Belov" ),
				QT_TRANSLATE_NOOP( "Task", "Developer" ),
				QLatin1String("null@deltaz.org") );
	setInfo( QT_TRANSLATE_NOOP("Plugin", "Weather plugin"),
				QT_TRANSLATE_NOOP("Plugin", "Plugin shows a current weather in your city."),
				PLUGIN_VERSION( 0, 0, 1, 0 ),
				ExtensionIcon( QIcon( ":/icons/weather.png" ) ) );
	setCapabilities(Loadable);
	ActionGenerator *gen = new ActionGenerator(QIcon(":/icons/weather.png"),
											   QT_TRANSLATE_NOOP("Weather", "Get weather"),
											   SLOT(getWeather()));
	MenuController::addAction<WContact>(gen);
	gen = new ActionGenerator(QIcon(":/icons/weather.png"),
							  QT_TRANSLATE_NOOP("Weather", "Get weather forecast"),
							  SLOT(getForecast()));
	MenuController::addAction<WContact>(gen);
}

bool WeatherPlugin::load()
{
	m_protocol = new WProtocol();

	return true;
}

bool WeatherPlugin::unload()
{
	if (m_protocol)
		delete m_protocol;
	return true;
}

QUTIM_EXPORT_PLUGIN( WeatherPlugin )

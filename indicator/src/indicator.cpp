/****************************************************************************
 * indicator.cpp
 *  Copyright © 2010, Vsevolod Velichko <torkvema@gmail.com>.
 *  Licence: GPLv3 or later
 *
 ****************************************************************************
 *                                                                          *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 ****************************************************************************/

#include "indicator.h"
#include <qutim/debug.h>
#include <qutim/icon.h>

IndicatorPlugin::IndicatorPlugin ()
{
}

void IndicatorPlugin::init ()
{
	qutim_sdk_0_3::ExtensionIcon icon("info");
	addAuthor(QT_TRANSLATE_NOOP("Author", "Vsevolod Velichko"),
			QT_TRANSLATE_NOOP("Task", "Developer"),
			QLatin1String("torkvema@gmail.com")
			);
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Indicator"),
			QT_TRANSLATE_NOOP("Plugin", "Ubuntu Indicator applet integration"),
			PLUGIN_VERSION(0, 0, 1, 0),
			icon
			);
	addExtension<IndicatorService>(QT_TRANSLATE_NOOP("Plugin", "Indicator"),
			QT_TRANSLATE_NOOP("Plugin", "Ubuntu Indicator applet integration")
			);
}

bool IndicatorPlugin::load ()
{
	qDebug() << "[Indicator] Plugin load";
	return true;
}

bool IndicatorPlugin::unload ()
{
	qDebug() << "[Indicator] Plugin unload";
	return false;
}

QUTIM_EXPORT_PLUGIN(IndicatorPlugin)

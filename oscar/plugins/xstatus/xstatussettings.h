/****************************************************************************
 *  xstatussettings.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef XSTATUSSETTINGS_H
#define XSTATUSSETTINGS_H

#include "settingsextension.h"

namespace qutim_sdk_0_3 {

namespace oscar {

class XStatusSettings : public QObject, public SettingsExtension
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_3::oscar::SettingsExtension)
public:
	XStatusSettings();
	void loadSettings(DataItem &item, Config cfg);
	void saveSettings(const DataItem &item, Config cfg);
};

} } // namespace qutim_sdk_0_3::oscar

#endif // XSTATUSSETTINGS_H

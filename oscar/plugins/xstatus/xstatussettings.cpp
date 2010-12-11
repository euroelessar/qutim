/****************************************************************************
 *  xstatussettings.cpp
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

#include "xstatussettings.h"
#include <qutim/config.h>

namespace qutim_sdk_0_3 {
namespace oscar {

XStatusSettings::XStatusSettings()
{
}

void XStatusSettings::loadSettings(DataItem &item, Config cfg)
{
	cfg.beginGroup("xstatus");
	item << DataItem("xstatusAutorequest", tr("Ask for XStatuses automatically"),
					 cfg.value("xstatusAutorequest", true));
	cfg.endGroup();
}

void XStatusSettings::saveSettings(const DataItem &item, Config cfg)
{
	cfg.beginGroup("xstatus");
	cfg.setValue("xstatusAutorequest", item.subitem("xstatusAutorequest").data<bool>());
	cfg.endGroup();
}

} } // namespace qutim_sdk_0_3::oscar

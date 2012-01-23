/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

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
	DataItem subitem("xstatusSettings", tr("Extended status"), QVariant());
	{
		subitem << DataItem("xstatusAutorequest",
							tr("Ask for XStatuses automatically"),
							cfg.value("xstatusAutorequest", true));
	}
	item.addSubitem(subitem);
	cfg.endGroup();
}

void XStatusSettings::saveSettings(const DataItem &item, Config cfg)
{
	DataItem subitem = item.subitem("xstatusSettings");
	cfg.beginGroup("xstatus");
	cfg.setValue("xstatusAutorequest", subitem.subitem("xstatusAutorequest").data<bool>());
	cfg.endGroup();
}

} } // namespace qutim_sdk_0_3::oscar


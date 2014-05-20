/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Vsevolod Velichko <torkvema@gmail.com>
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

#ifndef INDICATOR_GS5D2FLA
#define INDICATOR_GS5D2FLA
	
#include <qutim/plugin.h>
#include "indicatorservice.h"

class IndicatorPlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "org.qutim.Plugin")
	Q_CLASSINFO("DebugName", "Indicator")
	Q_CLASSINFO("Uses", "ChatLayer")
public:
	explicit IndicatorPlugin ();
	virtual void init();
	virtual bool load();
	virtual bool unload();
private:
	QPointer<IndicatorService> m_service;
};

#endif /* end of include guard: INDICATOR_GS5D2FLA */


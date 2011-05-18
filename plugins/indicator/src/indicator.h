/****************************************************************************
 * indicator.h
 *  Copyright © 2010-2011, Vsevolod Velichko <torkvema@gmail.com>.
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

#ifndef INDICATOR_GS5D2FLA
#define INDICATOR_GS5D2FLA
	
#include <qutim/plugin.h>
#include "indicatorservice.h"

class IndicatorPlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
public:
	explicit IndicatorPlugin ();
	virtual void init();
	virtual bool load();
	virtual bool unload();
private:
	QWeakPointer<IndicatorService> m_service;
};

#endif /* end of include guard: INDICATOR_GS5D2FLA */

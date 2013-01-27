/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexander Kazarin <boiler@co.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
** Copyright © 2012 Nicolay Izoderov <nico-izo@ya.ru>
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

#ifndef SCREENSHOTERPLUGIN_H
#define SCREENSHOTERPLUGIN_H

#include <qutim/shortcut.h>
#include <qutim/plugin.h>
#include "shoter.h"

using namespace qutim_sdk_0_3;
class Screenshoter : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName","Screenshoter")
public:
	Screenshoter();

protected:
	void resizeEvent(QResizeEvent *event);

private:
	virtual bool load();
	virtual bool unload();
	virtual void init();
	Shoter *m_shoter;
	GlobalShortcut *m_shortcut;

public slots:
	void initShoter();

};

#endif

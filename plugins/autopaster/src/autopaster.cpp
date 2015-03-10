/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2013 Tretyakov Roman <roman@trett.ru>
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

#include "autopaster.h"
#include <qqml.h>
#include <qutim/settingslayer.h>

using namespace qutim_sdk_0_3;

void AutoPaster::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Autopaster"),
			QT_TRANSLATE_NOOP("Plugin", "Paste your code to hosting"),
			PLUGIN_VERSION(0, 1, 0, 0));
	setCapabilities(Loadable);
	addAuthor(QLatin1String("trett"));
	addAuthor(QLatin1String("euroelessar"));
	qmlRegisterUncreatableType<AutoPasterHandler>("org.qutim.autopaster", 0, 4, "Handler", "It's internal type");
}

AutoPasterHandler *AutoPaster::handler()
{
	return m_handler;
}

bool AutoPaster::load()
{
	m_handler = new AutoPasterHandler();

	emit handlerChanged(m_handler);

	return true;
}

bool AutoPaster::unload()
{
	delete m_handler;
	m_handler = nullptr;

	emit handlerChanged(m_handler);

	return true;
}

QUTIM_EXPORT_PLUGIN(AutoPaster)

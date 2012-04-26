/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2012 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "formulaplugin.h"

using namespace qutim_sdk_0_3;

void FormulaPlugin::init()
{
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Formula"),
			QT_TRANSLATE_NOOP("Plugin", "Plugin replaces equations surrounded by $$ by tex images"),
			PLUGIN_VERSION(0, 1, 0, 0));
	setCapabilities(Loadable);
	addAuthor(QLatin1String("euroelessar"));
}

bool FormulaPlugin::load()
{
	m_handler.reset(new FormulaHandler);
	MessageHandler::registerHandler(m_handler.data(),
	                                QLatin1String("Formula"),
	                                qutim_sdk_0_3::MessageHandler::HighPriority - 0x100,
								    qutim_sdk_0_3::MessageHandler::HighPriority - 0x100);
	return true;
}

bool FormulaPlugin::unload()
{
	if (m_handler) {
		m_handler.reset(0);
		return true;
	}
	return false;
}

QUTIM_EXPORT_PLUGIN(FormulaPlugin)

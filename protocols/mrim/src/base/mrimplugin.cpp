/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Rusanov Peter <peter.rusanov@gmail.com>
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

#include "mrimplugin.h"
#include "mrimprotocol.h"
#include "../ui/wizards/mrimaccountwizard.h"
#include <qutim/debug.h>

#define MRIM_MAJOR_VER 0
#define MRIM_MINOR_VER 1
#define MRIM_SECMINOR_VER 0
#define MRIM_PATCH_VER 0

MrimPlugin::MrimPlugin()
{
	debug() << Q_FUNC_INFO;
}

void MrimPlugin::init()
{
    LocalizedString info = QT_TRANSLATE_NOOP("Plugin", "Module-based realization of Mail.Ru IM protocol");
    LocalizedString plugName = QT_TRANSLATE_NOOP("Plugin", "MRIM");

    setInfo(plugName, info,
            PLUGIN_VERSION(MRIM_MAJOR_VER, MRIM_MINOR_VER, MRIM_SECMINOR_VER, MRIM_PATCH_VER));

    addAuthor(QT_TRANSLATE_NOOP("Author", "Peter Rusanov"),
              QT_TRANSLATE_NOOP("Task", "Author"),
              QLatin1String("peter.rusanov@gmail.com"));

	addExtension(plugName, info,
				 new GeneralGenerator<MrimProtocol>(),
				 ExtensionIcon("im-mrim"));

	addExtension(QT_TRANSLATE_NOOP("Plugin", "MRIM account creator"),
				 QT_TRANSLATE_NOOP("Plugin", "Account creator for module-based realization of Mail.Ru IM protocol"),
				 new GeneralGenerator<MrimAccountWizard>(),
				 ExtensionIcon("im-mrim"));
}

bool MrimPlugin::load()
{
	debug() << Q_FUNC_INFO;
    return true;
}

bool MrimPlugin::unload()
{
	debug() << Q_FUNC_INFO;
    return false;
}

QUTIM_EXPORT_PLUGIN(MrimPlugin)


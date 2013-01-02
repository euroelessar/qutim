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

#include "contactmodelplugin.h"
#include "contactlistfrontmodel.h"
#include "contactlistaccountmodel.h"
#include "contactlistgroupmodel.h"
#include "contactlistplainmodel.h"
#include "contactlistseparatedmodel.h"

ContactModelPlugin::ContactModelPlugin()
{
}

void ContactModelPlugin::init()
{
    addAuthor("euroelessar");
    setInfo(QT_TRANSLATE_NOOP("Plugin", "Contact list model set"),
            QT_TRANSLATE_NOOP("Plugin", "Full-featured contact list model set"),
			QUTIM_VERSION);
	addExtension<ContactListFrontModel>(QT_TRANSLATE_NOOP("Plugin", "Contact list model"),
				 QT_TRANSLATE_NOOP("Plugin", "Full-featured contact list model"));
	addExtension<ContactListAccountModel>(QT_TRANSLATE_NOOP("Plugin", "Contact list model"),
				 QT_TRANSLATE_NOOP("Plugin", "Show accounts and contacts"));
	addExtension<ContactListGroupModel>(QT_TRANSLATE_NOOP("Plugin", "Contact list model"),
				 QT_TRANSLATE_NOOP("Plugin", "Show tags and contacts"));
	addExtension<ContactListPlainModel>(QT_TRANSLATE_NOOP("Plugin", "Contact list model"),
				 QT_TRANSLATE_NOOP("Plugin", "Show only contacts"));
	addExtension<ContactListSeparatedModel>(QT_TRANSLATE_NOOP("Plugin", "Contact list model"),
				 QT_TRANSLATE_NOOP("Plugin", "Show accounts, tags and contacts"));
}

bool ContactModelPlugin::load()
{
    return false;
}

bool ContactModelPlugin::unload()
{
    return true;
}

QUTIM_EXPORT_PLUGIN(ContactModelPlugin)

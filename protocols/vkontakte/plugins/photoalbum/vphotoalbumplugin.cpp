/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#include "vphotoalbumplugin.h"
#include <qutim/menucontroller.h>
#include "../../src/vcontact.h"
#include <qutim/icon.h>
#include <qutim/debug.h>
#include "vphotoview.h"

namespace Vkontakte
{

void VPhotoAlbumPlugin::init()
{
	debug() << Q_FUNC_INFO;
	setInfo(QT_TRANSLATE_NOOP("Plugin", "Photoalbum"),
			QT_TRANSLATE_NOOP("Plugin", "Vkontakte photoalbum support for qutIM"),
			PLUGIN_VERSION(0, 0, 1, 0));
	setCapabilities(Loadable);
	addAuthor(QLatin1String("sauron"));
}

bool VPhotoAlbumPlugin::load()
{
	debug() << Q_FUNC_INFO;
	static ActionGenerator wall_gen(Icon("camera-photo"),
									QT_TRANSLATE_NOOP("Vkontakte","View photoalbum"),
									this,
									SLOT(onViewPhotoTriggered(QObject*)));
	wall_gen.setType(ActionTypeContactList);
	MenuController::addAction<VContact>(&wall_gen);
	return true;
}

bool VPhotoAlbumPlugin::unload()
{
	return false;
}

void VPhotoAlbumPlugin::onViewPhotoTriggered(QObject *obj)
{
	VPhotoView *view = new VPhotoView(obj);
	centerizeWidget(view);
	view->show();
}

}

QUTIM_EXPORT_PLUGIN(Vkontakte::VPhotoAlbumPlugin);


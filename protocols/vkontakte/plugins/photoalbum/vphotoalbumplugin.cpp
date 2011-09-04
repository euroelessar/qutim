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
	addAuthor(QT_TRANSLATE_NOOP("Author", "Alexey Sidorov"),
			  QT_TRANSLATE_NOOP("Task", "Author"),
			  QLatin1String("sauron@citadelspb.com"),
			  QLatin1String("sauron.me")
			);
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

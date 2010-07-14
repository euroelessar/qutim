#include "qmlpopups.h"
#include <qutim/debug.h>
#include "backend.h"
#include <qutim/icon.h>

namespace QmlPopups 
{
	
	void QmlPopupsPlugin::init()
	{
	    debug() << Q_FUNC_INFO;
		setInfo(QT_TRANSLATE_NOOP("Plugin", "QmlPopups"),
				QT_TRANSLATE_NOOP("Plugin", "Modern popups, based on power of Qt Quick"),
			    PLUGIN_VERSION(0, 1, 0, 0));
		addAuthor(QT_TRANSLATE_NOOP("Author","Aleksey Sidorov"),
				  QT_TRANSLATE_NOOP("Task","Author"),
				  QLatin1String("sauron@citadelspb.com"),
				  QLatin1String("sauron.me"));
		addExtension<Backend>(QT_TRANSLATE_NOOP("plugin","Qml popups"),QT_TRANSLATE_NOOP("plugin","Modern popups, based on power of Qt Quick"));
	}

	bool QmlPopupsPlugin::load()
	{
		return true;
	}

	bool QmlPopupsPlugin::unload()
	{
		return false;
	}

}

QUTIM_EXPORT_PLUGIN(QmlPopups::QmlPopupsPlugin)

#include "iconsloaderimpl.h"
#include "src/modulemanagerimpl.h"
#include <QIcon>

namespace Core
{
	static CoreModuleHelper<IconLoaderImpl> icon_loader_static(
			QT_TRANSLATE_NOOP("Plugin", "Qt Icon Loader"),
			QT_TRANSLATE_NOOP("Plugin", "Default qutIM icon loader realization. Based on Qt classes")
			);

	IconLoaderImpl::IconLoaderImpl()
	{
	}

	QIcon IconLoaderImpl::loadIcon(const QString &name)
	{
		return QIcon::fromTheme(name);
	}

	QMovie *IconLoaderImpl::loadMovie(const QString &name)
	{
		return 0;
	}

	QString IconLoaderImpl::iconPath(const QString &name, StdSize iconSize)
	{
		Q_UNUSED(name);
		Q_UNUSED(iconSize);
		return QString();
	}

	QString IconLoaderImpl::moviePath(const QString &name, StdSize iconSize)
	{
		Q_UNUSED(name);
		Q_UNUSED(iconSize);
		return QString();
	}
}

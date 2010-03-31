#include "iconsloaderimpl.h"
#include "libqutim/configbase.h"
#include "src/modulemanagerimpl.h"
#include "3rdparty/q-xdg/src/xdgiconmanager.h"

namespace Core
{
	Q_GLOBAL_STATIC_WITH_ARGS(XdgIconManager, iconManager, (QList<QDir>() << QDir("/usr/share/qutim")))

	static const XdgIconTheme *this_iconTheme = 0;

	const XdgIconTheme *iconTheme()
	{
		if (!this_iconTheme) {
			const XdgIconTheme *theme = iconManager()->defaultTheme();
			QString id = Config().group("appearance").value<QString>("theme", theme ? theme->name() : "");
			this_iconTheme = iconManager()->themeById(id);
			if (!this_iconTheme && theme)
				this_iconTheme = theme;
			else if (!this_iconTheme) {
				this_iconTheme = iconManager()->themeById(QLatin1String("oxygen"));
				if (!this_iconTheme)
					this_iconTheme = iconManager()->themeById(QLatin1String("hicolor"));
			}
		}
		return this_iconTheme;
	}

	static CoreModuleHelper<IconLoaderImpl> icon_loader_static(
			QT_TRANSLATE_NOOP("Plugin", "Xdg Icon Loader"),
			QT_TRANSLATE_NOOP("Plugin", "Default qutIM icon loader implementation. Based on Q-XDG library")
			);

	IconLoaderImpl::IconLoaderImpl()
	{
	}

	QIcon IconLoaderImpl::loadIcon(const QString &name)
	{
		return iconTheme() ? iconTheme()->getIcon(name) : QIcon();
	}

	QMovie *IconLoaderImpl::loadMovie(const QString &name)
	{
		return 0;
	}

	QString IconLoaderImpl::iconPath(const QString &name, uint iconSize)
	{
		return iconTheme() ? iconTheme()->getIconPath(name, iconSize) : QString();
	}

	QString IconLoaderImpl::moviePath(const QString &name, uint iconSize)
	{
		Q_UNUSED(name);
		Q_UNUSED(iconSize);
		return QString();
	}
}

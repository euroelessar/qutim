#include "iconsloaderimpl.h"
#include <qutim/configbase.h>
#include "3rdparty/q-xdg/src/xdgiconmanager.h"
#include <qutim/systeminfo.h>

namespace Core
{
	Q_GLOBAL_STATIC_WITH_ARGS(XdgIconManager, iconManager,
							  (QList<QDir>()
							   << SystemInfo::getDir(SystemInfo::ShareDir)
							   << SystemInfo::getDir(SystemInfo::SystemShareDir)))

	static const XdgIconTheme *this_iconTheme = 0;

	const XdgIconTheme *iconTheme()
	{
		if (!this_iconTheme) {
			const XdgIconTheme *theme = 0; //iconManager()->defaultTheme();
			QString id = Config().group("appearance").value<QString>("theme", theme ? theme->id() : QString());
			this_iconTheme = iconManager()->themeById(id);
			if (!this_iconTheme && theme && theme->id() != "hicolor") {
				this_iconTheme = theme;
			} else if (!this_iconTheme) {
				this_iconTheme = iconManager()->themeById(QLatin1String("oxygen"));
				if (!this_iconTheme)
					this_iconTheme = iconManager()->themeById(QLatin1String("hicolor"));
			}
			// We don't want usually to use "hicolor"
			if (this_iconTheme->id() == "hicolor") {
				QStringList themes = iconManager()->themeIds();
				themes.removeOne("hicolor");
				if (!themes.isEmpty())
					this_iconTheme = iconManager()->themeById(themes.at(qrand() % themes.size()));
			}
		}
		return this_iconTheme;
	}

	IconLoaderImpl::IconLoaderImpl()
	{
	}

	QIcon IconLoaderImpl::loadIcon(const QString &name)
	{
		return iconTheme() ? iconTheme()->getIcon(name) : QIcon();
	}

	QMovie *IconLoaderImpl::loadMovie(const QString &name)
	{
		Q_UNUSED(name);
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

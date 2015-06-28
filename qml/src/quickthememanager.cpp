#include "quickthememanager.h"
#include "thememanager.h"

namespace qutim_sdk_0_3 {

QuickThemeManager::QuickThemeManager(QObject *parent) :
	QObject(parent)
{
}

QString QuickThemeManager::path(const QString &category, const QString &themeName)
{
	return ThemeManager::path(category, themeName);
}

QStringList QuickThemeManager::list(const QString &category)
{
	return ThemeManager::list(category);
}

} // namespace qutim_sdk_0_3

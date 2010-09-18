#include "thememanager.h"
#include "systeminfo.h"

namespace qutim_sdk_0_3
{

	ThemeManager::ThemeManager()
	{
	}

	ThemeManager::~ThemeManager()
	{
	}

	static inline QString getThemePath(QDir shareDir, const QString &themeName , const QString &category)
	{
		if (shareDir.cd(category) && shareDir.cd(themeName))
			return shareDir.absolutePath();
		else
			return QString();
	}

	QString ThemeManager::path(const QString& category, const QString &themeName)
	{
		QString themePath = getThemePath(SystemInfo::getDir(SystemInfo::ShareDir),themeName,category);
		if (themePath.isEmpty()) {
			themePath = getThemePath(SystemInfo::getDir(SystemInfo::SystemShareDir),themeName,category);
			if (themePath.isEmpty())
				themePath = getThemePath(QDir(":/"),themeName,category);
		}
		return themePath;
	}

	static inline QStringList listThemes(QDir shareDir, const QString &category)
	{
		if (shareDir.cd(category))
			return shareDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		else
			return QStringList();
	}

	QStringList ThemeManager::list(const QString &category)
	{
		QStringList theme_list;
		theme_list << listThemes(SystemInfo::getDir(qutim_sdk_0_3::SystemInfo::ShareDir),category);
		theme_list << listThemes(SystemInfo::getDir(qutim_sdk_0_3::SystemInfo::SystemShareDir),category);
		theme_list << listThemes(QDir(":/"),category);
		theme_list.removeDuplicates();
		return theme_list;
	}

}

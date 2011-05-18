#include "thememanager.h"
#include "systeminfo.h"
#include <QMultiMap>

namespace qutim_sdk_0_3
{
	class ThemeManagerData
	{
	public:
		ThemeManagerData() { paths << QDir(QLatin1String(":/")); }
		QList<QDir> paths;
		QMultiMap<QString,QDir> categoryPaths;
	};
	
	Q_GLOBAL_STATIC(ThemeManagerData, data)

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
		foreach (const QDir &dir, data()->paths)
			theme_list << listThemes(dir, category);
		foreach (const QDir &dir, data()->categoryPaths.values(category))
			theme_list << listThemes(dir, category);
		theme_list.removeDuplicates();
		return theme_list;
	}
	
	void addPath(const QString &path_, const QString &category)
	{
	}

	QList<QDir> ThemeManager::categoryDirs(const QString &category)
	{
		QList<QDir> list;
		QDir dir = SystemInfo::getDir(qutim_sdk_0_3::SystemInfo::ShareDir);
		if (dir.cd(category))
			list << dir;

		dir = SystemInfo::getDir(qutim_sdk_0_3::SystemInfo::SystemShareDir);
		if (dir.cd(category))
			list << dir;

		dir = QDir(":/" + category);
		if (dir.exists())
			list << dir;

		foreach (QDir dir, data()->paths) {
			if (dir.cd(category))
				list << dir;
		}

		foreach (QDir dir, data()->categoryPaths) {
			if (dir.cd(category))
				list << dir;
		}

		return list;
	}
}

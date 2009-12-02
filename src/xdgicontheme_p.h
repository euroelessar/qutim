#ifndef XDGICONTHEME_P_H
#define XDGICONTHEME_P_H

#include "xdgicontheme.h"

struct XdgIconDir
{
	enum Type
	{
		Fixed = 0,
		Scalable = 1,
		Threshold = 2
	};

	QString path;
	uint size;
	Type type;
	uint maxsize;
	uint minsize;
	uint threshold;
};

class XdgIconThemePrivate
{
public:
	QString id;
	QString name;
	QVector<QDir> basedirs;
	QStringList parentNames;
	QVector<XdgIconDir> subdirs;
	QVector<const XdgIconTheme *> parents;
	mutable QMap<QString, QString> cache;

	QString findIcon(const QString& name, uint size) const;
	QString lookupIconRecursive(const QString& name, uint size) const;
	QString lookupFallbackIcon(const QString& name) const;
	bool dirMatchesSize(const XdgIconDir &dir, uint size) const;
	unsigned dirSizeDistance(const XdgIconDir& dir, uint size) const;
};

#endif // XDGICONTHEME_P_H

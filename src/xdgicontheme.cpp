/*
    Copyright (C) 2009 Maia Kozheva <sikon@ubuntu.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <limits>
#include <QtCore/QSettings>
#include "xdgicontheme_p.h"

namespace
{
	const char *exts[] = { ".png", ".svg", ".xpm" };
	const int extCount = sizeof(exts) / sizeof(char *);
}

QString XdgIconThemePrivate::findIcon(const QString& name, uint size) const
{
	QString filename = lookupIconRecursive(name, size);

	if (!filename.isEmpty())
		return filename;

	return lookupFallbackIcon(name);
}

QString XdgIconThemePrivate::lookupIconRecursive(const QString& name, uint size) const
{
	// Look for an exact size match first, per specification
	foreach (const XdgIconDir& dirdata, subdirs) {
		if (dirMatchesSize(dirdata, size)) {

			// TODO: May be we should use QStringBuilder?
			// Release of Qt 4.6 has already been...
			QString path = id;
			path += QLatin1Char('/');
			path += dirdata.path;
			path += QLatin1Char('/');
			path += name;

			foreach (QDir basedir, basedirs) {
				for (int k = 0; k < extCount; k++) {
					path += QLatin1String(exts[k]);
					QString filename = basedir.absoluteFilePath(path);

					if (QFile::exists(filename))
						return filename;
					path.chop(4);
				}
			}
		}
	}

	uint mindist = std::numeric_limits<uint>::max();
	QString closestFilename;

	foreach(const XdgIconDir &dirdata, subdirs)
	{
		uint distance = dirSizeDistance(dirdata, size);

		if(distance >= mindist)
			continue;

		QString path = id;
		path += QLatin1Char('/');
		path += dirdata.path;
		path += QLatin1Char('/');
		path += name;

		// Here we look for the first match in the given subdir.
		// If found, we decrease the mindist, immediately exit
		// the inner loops and move to the next subdir
		for (int j = 0; j < basedirs.size(); j++) {
			bool found = false;

			for (int k = 0; k < extCount; k++) {
				path += QLatin1String(exts[k]);
				QString filename = QDir(basedirs.at(j)).absoluteFilePath(path);

				if (QFile::exists(filename)) {
					closestFilename = filename;
					mindist = distance;
					found = true;
					break;
				}
				path.chop(4);
			}

			if(found)
				break;
		}

		// mindist can't be below 1, so if we reached that,
		// it's pointless to look further
		if (mindist == 1)
			break;
	}

	if (!closestFilename.isEmpty())
		return closestFilename;

	foreach (const XdgIconTheme *parent, parents) {
		closestFilename = parent->d_func()->lookupIconRecursive(name, size);

		if (!closestFilename.isEmpty())
			return closestFilename;
	}

	return QString();
}

QString XdgIconThemePrivate::lookupFallbackIcon(const QString& name) const
{
	for (int i = 0; i < basedirs.size(); i++) {
		QDir dir(basedirs.at(i));

		for (int j = 0; j < extCount; j++) {
			QString fullname = dir.absoluteFilePath(name + exts[j]);

			if (QFile::exists(fullname))
				return fullname;
		}
	}

	return QString();
}

bool XdgIconThemePrivate::dirMatchesSize(const XdgIconDir &dir, uint size) const
{
	switch (dir.type) {
	case XdgIconDir::Fixed:
		return size == dir.size;
	case XdgIconDir::Scalable:
		return (size >= dir.minsize) && (size <= dir.maxsize);
	default:
		return (size >= dir.size - dir.threshold) && (size <= dir.size + dir.threshold);
	}
	Q_ASSERT(!"New directory type?..");
	return false;
}

uint XdgIconThemePrivate::dirSizeDistance(const XdgIconDir& dir, uint size) const
{
	switch (dir.type) {
		case XdgIconDir::Fixed:
			return abs(dir.size - size);
		case XdgIconDir::Scalable:
			if(size < dir.minsize)
				return dir.minsize - size;
			if(size > dir.maxsize)
				return size - dir.maxsize;
			return 0;
		case XdgIconDir::Threshold:
			if(size < dir.size - dir.threshold)
				return dir.size - dir.threshold - size;
			if(size > dir.size + dir.threshold)
				return size - dir.size - dir.threshold;
			return 0;
	}
	Q_ASSERT(!"New directory type?..");
	return 0;
}

XdgIconTheme::XdgIconTheme(const QVector<QDir>& basedirs, const QString& id, const QString &indexFileName)
		: p(new XdgIconThemePrivate)
{
	Q_D(XdgIconTheme);

	d->id = id;
	d->basedirs = basedirs;
    QSettings settings(indexFileName, QSettings::IniFormat);

	settings.beginGroup(QLatin1String("Icon Theme"));
	d->parentNames = settings.value(QLatin1String("Inherits")).toStringList();
	QStringList subdirList = settings.value(QLatin1String("Directories")).toStringList();
    settings.endGroup();

	for (QListIterator<QString> it(subdirList); it.hasNext();) {
        // The defaults are dictated by the FDO specification
		d->subdirs.append(XdgIconDir());
		XdgIconDir &dirdata = d->subdirs.last();

        dirdata.path = it.next();
        settings.beginGroup(dirdata.path);
		dirdata.size = settings.value(QLatin1String("Size")).toUInt();
		dirdata.maxsize = settings.value(QLatin1String("MaxSize"), dirdata.size).toUInt();
		dirdata.minsize = settings.value(QLatin1String("MinSize"), dirdata.size).toUInt();
		dirdata.threshold = settings.value(QLatin1String("Threshold"), 2).toUInt();
		QString type = settings.value(QLatin1String("Type"), QLatin1String("Threshold")).toString();
        settings.endGroup();

		if (type == QLatin1String("Fixed"))
			dirdata.type = XdgIconDir::Fixed;
		else if (type == QLatin1String("Scalable"))
			dirdata.type = XdgIconDir::Scalable;
		else
			dirdata.type = XdgIconDir::Threshold;
    }
}

XdgIconTheme::~XdgIconTheme()
{
	delete p;
}

QString XdgIconTheme::id() const
{
	return d_func()->id;
}

QString XdgIconTheme::name() const
{
	return d_func()->name;
}

QStringList XdgIconTheme::parentNames() const
{
	return d_func()->parentNames;
}

void XdgIconTheme::addParent(const XdgIconTheme *parent)
{
	Q_D(XdgIconTheme);
	Q_ASSERT_X(parent, "XdgIconTheme::addParent", "Parent must be not null");
	d->parents.append(parent);
}

QString XdgIconTheme::getIconPath(const QString& name, uint size) const
{
	Q_D(const XdgIconTheme);

	QString key = QChar(size);
	key += QLatin1Char(' ');
	key += name;

	// FIXME: We should use QCache, it's rather faster than QMap
	QMap<QString, QString>::const_iterator it = d->cache.constFind(key);
	if (it != d->cache.constEnd())
		return it.value();

	QString filename = d->findIcon(name, size);

	if (!filename.isEmpty())
		d->cache.insert(key, filename);

    return filename;
}

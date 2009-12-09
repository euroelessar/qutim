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
#include <QtCore/QSet>
#include "xdgicontheme_p.h"

namespace
{
    const char *exts[] = { ".png", ".svg", ".svgz", ".svg.gz", ".xpm" };
    const int extCount = sizeof(exts) / sizeof(char *);
}

const XdgIconEntry *XdgIconData::findEntry(uint size) const
{
    if (entries.isEmpty())
        return 0;

    // Look for an exact size match first, per specification
    for (int i = 0; i < entries.size(); i++) {
        if (XdgIconThemePrivate::dirMatchesSize(*entries[i].dir, size)) {
            return &entries[i];
        }
    }

    // Then find the closest size
    uint mindist = 0;
    const XdgIconEntry *entry = 0;
    for (int i = 0; i < entries.size(); i++) {
        uint distance = XdgIconThemePrivate::dirSizeDistance(*entries[i].dir, size);

        if (!entry || distance < mindist) {
            mindist = distance;
            entry = &entries[i];
        }
    }

    return entry;
}

bool XdgIconData::destroy()
{
    entries.clear();
    name.clear();
    theme = 0;
    return !ref;
}

XdgIconData *XdgIconThemePrivate::findIcon(const QString &name) const
{
    QString key = id;
    key += QLatin1Char('\0');
    key += name;

    XdgIconData *data = 0;

    XdgIconDataHash::const_iterator it = cache.constFind(key);
    if (it != cache.constEnd()) {
        data = it.value();
    } else {
		QSet<const XdgIconThemePrivate*> themeSet;
		data = lookupIconRecursive(name, themeSet);
        cache.insert(key, data);
    }

    return data;
}

XdgIconData *XdgIconThemePrivate::lookupIconRecursive(const QString &originName,
													  QSet<const XdgIconThemePrivate*> &themeSet) const
{
    XdgIconData *data = 0;

	if (themeSet.contains(this))
		return data;
	themeSet.insert(this);

    QString name = originName;
    while (!name.isEmpty()) {
        if (data = tryCache(name))
            return data;

        foreach (const XdgIconDir &dirdata, subdirs) {
            QString path = id;
            path += QLatin1Char('/');
            path += dirdata.path;
            path += QLatin1Char('/');
            path += name;

            for (int j = 0; j < basedirs.size(); j++) {
                for (int k = 0; k < extCount; k++) {
                    path += QLatin1String(exts[k]);
                    QString filename = QDir(basedirs.at(j)).absoluteFilePath(path);

                    if (QFile::exists(filename)) {
                        if (!data) {
                            data = new XdgIconData;
                            data->theme = this;
                            data->name = name;
                            path.chop(qstrlen(exts[k]));
                        }
                        data->entries.append(XdgIconEntry(&dirdata, filename));
                        break;
                    }
                    path.chop(qstrlen(exts[k]));
                }
            }
        }
        if (data) {
            saveToCache(originName, data);
            return data;
        }
        int pos = name.lastIndexOf(QLatin1Char('-'));
        if (pos != -1) {
            name.truncate(pos);
        }
    }

    if (!data) {
        foreach (const XdgIconTheme *parent, parents) {
			data = parent->d_func()->lookupIconRecursive(name, themeSet);
            if (data) {
                saveToCache(originName, data);
                break;
            }
        }
    }

    if (!data)
        saveToCache(originName, 0);

    return data;
}

XdgIconData *XdgIconThemePrivate::tryCache(const QString &name) const
{
    QString key;
    key.reserve(id.size() + name.size() + 1);
    key += id;
    key += QLatin1Char('\0');
    key += name;

    return cache.value(key, 0);
}

void XdgIconThemePrivate::saveToCache(const QString &originName, XdgIconData *data) const
{
    QString name = originName;
    QString key = id;
    key += QLatin1Char('\0');
    while (!name.isEmpty() && (name.size() >= (data ? data->name.size() : 0))) {
        key += name;
        cache.insert(key, data);

        int pos = name.lastIndexOf(QLatin1Char('-'));
        if (pos == -1)
            return;

        key.truncate(id.size() + 1);
        name.truncate(pos);
    }
}

QString XdgIconThemePrivate::lookupFallbackIcon(const QString &name) const
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

bool XdgIconThemePrivate::dirMatchesSize(const XdgIconDir &dir, uint size)
{
    switch (dir.type) {
    case XdgIconDir::Fixed:
        return size == dir.size;
    case XdgIconDir::Scalable:
        return (size >= dir.minsize) && (size <= dir.maxsize);
    case XdgIconDir::Threshold:
        return (size >= dir.size - dir.threshold) && (size <= dir.size + dir.threshold);
    }
    Q_ASSERT(!"New directory type?..");
    return false;
}

uint XdgIconThemePrivate::dirSizeDistance(const XdgIconDir &dir, uint size)
{
    switch (dir.type) {
        case XdgIconDir::Fixed:
            return qAbs(int(dir.size) - int(size));
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

XdgIconTheme::XdgIconTheme(const QVector<QDir> &basedirs, const QString &id, const QString &indexFileName)
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
    // There sometimes equal values for different keys, i.e. because of fallback
    QSet<XdgIconData *> allData = QSet<XdgIconData *>::fromList(p->cache.values());
    foreach (XdgIconData *data, allData) {
        if (data->theme == p && data->destroy())
            delete data;
    }
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
	if (!d->parents.contains(parent))
		d->parents.append(parent);
}

QString XdgIconTheme::getIconPath(const QString &name, uint size) const
{
    Q_D(const XdgIconTheme);

    XdgIconData *data = d->findIcon(name);
    const XdgIconEntry *entry = data ? data->findEntry(size) : 0;
    return entry ? entry->path : QString();
}

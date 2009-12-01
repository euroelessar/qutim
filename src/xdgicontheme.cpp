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
#include "xdgicontheme.h"

namespace
{
    const int extCount = 3;
    const char *exts[] = { "png", "svg", "xpm" };
}

XdgIconTheme::XdgIconTheme(const QVector<QDir>& basedirs, const QString& id, const QString &indexFileName)
        : _id(id), _basedirs(basedirs)
{
    QSettings settings(indexFileName, QSettings::IniFormat);

    settings.beginGroup("Icon Theme");
    _parentNames = settings.value("Inherits").toStringList();
    QStringList subdirList = settings.value("Directories").toStringList();
    settings.endGroup();

    for (QListIterator<QString> it(subdirList); it.hasNext(); )
    {
        // The defaults are dictated by the FDO specification
        _subdirs.append(IconDir());
        IconDir& dirdata = _subdirs.last();

        dirdata.path = it.next();
        settings.beginGroup(dirdata.path);
        dirdata.size = settings.value("Size").toUInt();
        dirdata.maxsize = settings.value("MaxSize", dirdata.size).toUInt();
        dirdata.minsize = settings.value("MinSize", dirdata.size).toUInt();
        dirdata.threshold = settings.value("Threshold", 2).toUInt();
        QString type = settings.value("Type", "Threshold").toString();
        settings.endGroup();

        if (type == "Fixed")
        {
            dirdata.type = IconDir::Fixed;
        }
        else if (type == "Scalable")
        {
            dirdata.type = IconDir::Scalable;
        }
        else
        {
            dirdata.type = IconDir::Threshold;
        }
    }
}

QString XdgIconTheme::findIcon(const QString& name, uint size) const
{
    QString filename = lookupIconRecursive(name, size);

    if(!filename.isEmpty())
    {
        return filename;
    }

    return lookupFallbackIcon(name);
}

QString XdgIconTheme::lookupIconRecursive(const QString& name, uint size) const
{
    // Look for an exact size match first, per specification
    foreach (const IconDir& dirdata, _subdirs)
    {
        if (dirMatchesSize(dirdata, size))
        {
            foreach (QDir basedir, _basedirs)
            {
                for (int k = 0; k < extCount; k++)
                {
                    QString filename = basedir.absoluteFilePath
                            (_id + '/' + dirdata.path + '/' + name + '.' + exts[k]);

                    if (QFile::exists(filename))
                        return filename;
                }
            }
        }
    }

    uint mindist = std::numeric_limits<uint>::max();
    QString closestFilename;

    foreach(const IconDir& dirdata, _subdirs)
    {
        uint distance = dirSizeDistance(dirdata, size);

        if(distance >= mindist)
            continue;

        // Here we look for the first match in the given subdir.
        // If found, we decrease the mindist, immediately exit
        // the inner loops and move to the next subdir
        for (int j = 0; j < _basedirs.size(); j++)
        {
            bool found = false;

            for (int k = 0; k < extCount; k++)
            {
                QString filename = QDir(_basedirs.at(j)).absoluteFilePath
                        (_id + '/' + dirdata.path + '/' + name + '.' + exts[k]);

                if (QFile::exists(filename))
                {
                    closestFilename = filename;
                    mindist = distance;
                    found = true;
                    break;
                }
            }

            if(found)
                break;
        }

        // mindist can't be below 1, so if we reached that,
        // it's pointless to look further
        if(mindist == 1)
            break;
    }

    if(!closestFilename.isEmpty())
    {
        return closestFilename;
    }

    foreach(const XdgIconTheme *parent, _parents)
    {
        closestFilename = parent->lookupIconRecursive(name, size);

        if(!closestFilename.isEmpty())
        {
            return closestFilename;
        }
    }

    return QString();
}

QString XdgIconTheme::lookupFallbackIcon(const QString& name) const
{
    for (int i = 0; i < _basedirs.size(); i++)
    {
        QDir dir(_basedirs.at(i));

        for (int j = 0; j < extCount; j++)
        {
            QString fullname = dir.absoluteFilePath(name + '.' + exts[j]);

            if (QFile::exists(fullname))
                return fullname;
        }
    }

    return QString();
}

bool XdgIconTheme::dirMatchesSize(const IconDir &dir, uint size) const
{
    switch (dir.type)
    {
    case IconDir::Fixed:
        return size == dir.size;
    case IconDir::Scalable:
        return (size >= dir.minsize) && (size <= dir.maxsize);
    default:
        return (size >= dir.size - dir.threshold) && (size <= dir.size + dir.threshold);
    }
}

uint XdgIconTheme::dirSizeDistance(const IconDir& dir, uint size) const
{
    switch (dir.type)
    {
        case IconDir::Fixed:
            return abs(dir.size - size);
        case IconDir::Scalable:
            if(size < dir.minsize)
                return dir.minsize - size;
            if(size > dir.maxsize)
                return size - dir.maxsize;
            return 0;
        case IconDir::Threshold:
            if(size < dir.size - dir.threshold)
                return dir.size - dir.threshold - size;
            if(size > dir.size + dir.threshold)
                return size - dir.size - dir.threshold;
            return 0;
    }

    // We shouldn't really get here - if we do, it's probably a bug
    return 0;
}

QString XdgIconTheme::getIconPath(const QString& name, uint size) const
{
    QString key = QString(size) + ' ' + name;

    if (_cache.contains(key))
    {
        return _cache[key];
    }

    QString filename = findIcon(name, size);

    if (!filename.isEmpty())
    {
        _cache.insert(key, filename);
    }

    return filename;
}

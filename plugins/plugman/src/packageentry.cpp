/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/

#include "packageentry.h"
#include <QStringList>
#include <QPixmap>

class PackageEntryData : public QSharedData
{
public:
	PackageEntryData() : status(PackageEntry::Invalid) {}
	
	Attica::Content content;
	PackageEntry::Status status;
	QPixmap smallPreview;
	QStringList installedFiles;
	QString installedVersion;
};


PackageEntry::PackageEntry() : d(new PackageEntryData)
{
}

PackageEntry::PackageEntry(const PackageEntry &o) : d(o.d)
{
}

PackageEntry &PackageEntry::operator =(const PackageEntry &o)
{
	d = o.d;
	return *this;
}

PackageEntry::~PackageEntry()
{
}

bool PackageEntry::isValid() const
{
	return d->content.isValid();
}

QString PackageEntry::id() const
{
	return d->content.id();
}

Attica::Content PackageEntry::content() const
{
	return d->content;
}
void PackageEntry::setContent(const Attica::Content &content)
{
	d->content = content;
}

PackageEntry::Status PackageEntry::status() const
{
	return d->status;
}

void PackageEntry::setStatus(PackageEntry::Status status)
{
	d->status = status;
}

QPixmap PackageEntry::smallPreview() const
{
	return d->smallPreview;
}

void PackageEntry::setSmallPreview(const QPixmap &preview)
{
	d->smallPreview = preview;
}

QStringList PackageEntry::installedFiles() const
{
	return d->installedFiles;
}

void PackageEntry::setInstalledFiles(const QStringList &files)
{
	d->installedFiles = files;
}

QString PackageEntry::installedVersion() const
{
	return d->installedVersion;
}

void PackageEntry::setInstalledVersion(const QString &version)
{
	d->installedVersion = version;
}


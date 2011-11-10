/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef PACKAGEENTRY_H
#define PACKAGEENTRY_H

#include <attica/content.h>
#include <QMetaType>

class PackageEntryData;

class PackageEntry
{
public:
	typedef QList<PackageEntry> List;
	enum Status
	{
		Invalid = -1,
		Installable,
		Updateable,
		Installing,
		Updating,
		Installed
	};
	
	PackageEntry();
	PackageEntry(const PackageEntry &o);
	PackageEntry &operator =(const PackageEntry &o);
	~PackageEntry();
	
	bool isValid() const;
	QString id() const;
	Attica::Content content() const;
	void setContent(const Attica::Content &content);
	Status status() const;
	void setStatus(Status status);
	QPixmap smallPreview() const;
	void setSmallPreview(const QPixmap &preview);
	QStringList installedFiles() const;
	void setInstalledFiles(const QStringList &files);
	QString installedVersion() const;
	void setInstalledVersion(const QString &version);
private:
	QExplicitlySharedDataPointer<PackageEntryData> d;
};

Q_DECLARE_METATYPE(PackageEntry)

#endif // PACKAGEENTRY_H


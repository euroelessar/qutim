/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2011 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
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
	
	PackageEntry();
	PackageEntry(const PackageEntry &o);
	PackageEntry &operator =(const PackageEntry &o);
	~PackageEntry();
	
	bool isValid() const;
	QString id() const;
	Attica::Content content() const;
	void setContent(const Attica::Content &content);
	QStringList installedFiles() const;
	void setInstalledFiles(const QStringList &files);
	QString installedVersion() const;
	void setInstalledVersion(const QString &version);
private:
	QExplicitlySharedDataPointer<PackageEntryData> d;
};

Q_DECLARE_METATYPE(PackageEntry)

#endif // PACKAGEENTRY_H

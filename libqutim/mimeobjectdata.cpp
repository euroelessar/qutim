/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#include "mimeobjectdata.h"
#include <QStringList>

namespace qutim_sdk_0_3
{
	class MimeObjectDataPrivate
	{		
	public:
		QPointer<QObject> object;
	};
	
	MimeObjectData::MimeObjectData() : d_ptr(new MimeObjectDataPrivate)
	{
	}
	
	MimeObjectData::~MimeObjectData()
	{
	}
	
	void MimeObjectData::setObject(QObject *obj)
	{
		d_func()->object = obj;
	}
	
	QObject *MimeObjectData::object() const
	{
		return d_func()->object;
	}
	
	QString MimeObjectData::objectMimeType()
	{
		return QLatin1String("application/qutim-object");
	}
	
	bool MimeObjectData::hasFormat(const QString &mimetype) const
	{
		if (mimetype == QLatin1String("application/qutim-object"))
			return !d_func()->object.isNull();
		return QMimeData::hasFormat(mimetype);
	}
	
	QStringList MimeObjectData::formats() const
	{
		QStringList list = QMimeData::formats();
		if (d_func()->object)
			list << QLatin1String("application/qutim-object");
		return list;
	}
	
	QVariant MimeObjectData::retrieveData(const QString &mimetype,
										  QVariant::Type preferredType) const
	{
		return QMimeData::retrieveData(mimetype, preferredType);
	}
}

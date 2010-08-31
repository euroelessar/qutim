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

#ifndef MIMEOBJECTDATA_H
#define MIMEOBJECTDATA_H

#include <QMimeData>
#include <QPointer>
#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	class MimeObjectDataPrivate;
	
	class LIBQUTIM_EXPORT MimeObjectData : public QMimeData
	{
		Q_OBJECT
		Q_DECLARE_PRIVATE(MimeObjectData)
	public:
		MimeObjectData();
		~MimeObjectData();
		
		void setObject(QObject *obj);
		QObject *object() const;
		static QString objectMimeType();
		
		virtual bool hasFormat(const QString &mimetype) const;
		virtual QStringList formats() const;
	protected:
		virtual QVariant retrieveData(const QString &mimetype,
									  QVariant::Type preferredType) const;
	private:
		QScopedPointer<MimeObjectDataPrivate> d_ptr;
	};
}

#endif // MIMEOBJECTDATA_H

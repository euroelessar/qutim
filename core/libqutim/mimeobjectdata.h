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


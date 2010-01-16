/****************************************************************************
 *  jsonfile.h
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

#ifndef JSONFILE_H
#define JSONFILE_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	struct JsonFilePrivate;

	class LIBQUTIM_EXPORT JsonFile
	{
		Q_DISABLE_COPY(JsonFile)
	public:
		enum OpenMode { ReadOnly, ReadWrite };
		JsonFile(const QString &name = QString());
		virtual ~JsonFile();
		void setFileName(const QString &name);
		bool load(QVariant &variant);
		bool save(const QVariant &variant);
//		bool open(OpenMode mode = ReadOnly);
//		QStringList keys();
//		bool enterKey(const QString &key);
//		QVariant readValue(const QString &key);
	private:
		JsonFilePrivate *p;
	};
}

#endif // JSONFILE_H

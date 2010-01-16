/****************************************************************************
 *  configbackend.h
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

#ifndef CONFIGBACKEND_H
#define CONFIGBACKEND_H

#include "libqutim_global.h"
#include <QMap>
#include <QVector>
#include <QVariant>
#include <QSharedPointer>

namespace qutim_sdk_0_3
{
	struct LIBQUTIM_EXPORT ConfigEntry
	{
		typedef QSharedPointer<ConfigEntry>    Ptr;
		typedef QWeakPointer<ConfigEntry>      WeakPtr;
		typedef QMap<QString,ConfigEntry::Ptr> EntryMap;
		typedef QVector<ConfigEntry::Ptr>      EntryArray;
		enum Type { Invalid = 0x00, Root = 0x01, Map = 0x02,
					Array = 0x04, AnyGroup = Map | Array, Value = 0x08 };
		Q_DECLARE_FLAGS(Flags, Type)
		inline ConfigEntry() : type(AnyGroup), deleted(false), dirty(false) {}
		inline ~ConfigEntry() { remove(); }
		bool remove();
		Flags type;
		bool deleted;
		bool dirty;
		QAtomicInt ref;
		EntryMap map;
		EntryArray array;
		QVariant value;
	};

	struct ConfigBackendPrivate;

	class LIBQUTIM_EXPORT ConfigBackend : public QObject
	{
		Q_OBJECT
	public:
		ConfigBackend();
		virtual ~ConfigBackend();
		virtual ConfigEntry::Ptr parse(const QString &file) = 0;
		virtual void generate(const QString &file, const ConfigEntry::Ptr &entry) = 0;
		QByteArray backendName() const;
	protected:
		QIODevice *openDefault(const QString &file, QIODevice::OpenMode mode);
		virtual void virtual_hook(int id, void *data);
	private:
		QScopedPointer<ConfigBackendPrivate> p;
	};
}

#endif // CONFIGBACKEND_H

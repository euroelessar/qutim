/****************************************************************************
 *  extensioninfo.h
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

#ifndef EXTENSIONINFO_H
#define EXTENSIONINFO_H

#include "personinfo.h"
#include "extensionicon.h"
#include "objectgenerator.h"
#include <QMetaType>
#include <QtGui/QIcon>

namespace qutim_sdk_0_3
{
class ExtensionInfoData;

class LIBQUTIM_EXPORT ExtensionInfo
{
public:
	ExtensionInfo(const LocalizedString &name = LocalizedString(),
				  const LocalizedString &description = LocalizedString(),
				  const ObjectGenerator *generator = 0, ExtensionIcon icon = ExtensionIcon());
	ExtensionInfo(const ExtensionInfo &other);
	~ExtensionInfo();
	ExtensionInfo &operator =(const ExtensionInfo &other);
	ExtensionInfo &setName(const LocalizedString &name);
	ExtensionInfo &setDescription(const LocalizedString &description);
	ExtensionInfo &setIcon(const ExtensionIcon &icon);
	ExtensionInfo &setGenerator(const ObjectGenerator *generator);
	QList<PersonInfo> authors() const;
	LocalizedString name() const;
	LocalizedString description() const;
	const ObjectGenerator *generator() const;
	ExtensionIcon icon() const;
private:
	QSharedDataPointer<ExtensionInfoData> d;
public:
	typedef ExtensionInfoData Data;
	Data *data() const;
};

typedef QList<ExtensionInfo> ExtensionInfoList;

LIBQUTIM_EXPORT ExtensionInfoList extensionList();

//	TODO: Add possibility for getting ExtensionInfoList of avalaible modules
//
//	LIBQUTIM_EXPORT ExtensionInfoList extensionList(const QMetaObject *module);
//	LIBQUTIM_EXPORT ExtensionInfoList extensionList(const char *iid);
//	template<typename T> inline ExtensionInfoList extensionList(const QObject *)
//	{ return moduleGenerators(&T::staticMetaObject); }
//	template<typename T> inline ExtensionInfoList extensionList(const void *)
//	{ return moduleGenerators(qobject_interface_iid<T *>()); }
//	template<typename T> inline ExtensionInfoList moduleGenerators()
//	{ return extensionList<T>(reinterpret_cast<T *>(0)); }
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::ExtensionInfo)

#endif // EXTENSIONINFO_H

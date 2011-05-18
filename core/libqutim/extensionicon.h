/****************************************************************************
 *  extensionicon.h
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

#ifndef EXTENSIONICON_H
#define EXTENSIONICON_H

#include "libqutim_global.h"
#include <QtGui/QIcon>
#include <QtCore/QSharedData>
#include <QMetaType>

namespace qutim_sdk_0_3
{
class ExtensionIconData;

class LIBQUTIM_EXPORT ExtensionIcon
{
public:
	ExtensionIcon(const QString &name = QString());
	ExtensionIcon(const QIcon &icon);
	ExtensionIcon(const ExtensionIcon &other);
	~ExtensionIcon();
	ExtensionIcon &operator =(const ExtensionIcon &other);
	operator QIcon() const { return toIcon(); }
	QIcon toIcon() const;
	QString name() const;
private:
	QSharedDataPointer<ExtensionIconData> p;
};
}

Q_DECLARE_METATYPE(qutim_sdk_0_3::ExtensionIcon);

#endif // EXTENSIONICON_H

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

#ifndef EXTENSIONICON_H
#define EXTENSIONICON_H

#include "libqutim_global.h"
#include <QIcon>
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


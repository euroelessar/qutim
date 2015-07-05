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

#include "extensionicon.h"
#include "icon.h"
#include "objectgenerator.h"

namespace qutim_sdk_0_3
{
class ExtensionIconData : public QSharedData
{
public:
	ExtensionIconData() {}
	ExtensionIconData(const ExtensionIconData &o) : QSharedData(o), name(o.name), icon(o.icon) {}
	mutable QString name;
	mutable QIcon icon;
};

ExtensionIcon::ExtensionIcon(const QString &name) : p(new ExtensionIconData)
{
	p->name = name;
}

ExtensionIcon::ExtensionIcon(const QIcon &icon) : p(new ExtensionIconData)
{
	p->icon = icon;
}

ExtensionIcon::ExtensionIcon(const ExtensionIcon &other) : p(other.p)
{
}

ExtensionIcon::~ExtensionIcon()
{
}

ExtensionIcon &ExtensionIcon::operator =(const ExtensionIcon &other)
{
	p = other.p;
	return *this;
}

QIcon ExtensionIcon::toIcon() const
{
	if (p->icon.isNull() && !p->name.isEmpty() && ObjectGenerator::isInited())
		p->icon = Icon(p->name);
	return p->icon;
}

QString ExtensionIcon::name() const
{
	return p->name;
}
}


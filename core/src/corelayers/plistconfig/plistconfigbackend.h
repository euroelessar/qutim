/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef PLISTCONFIGBACKEND_H
#define PLISTCONFIGBACKEND_H

#include <qutim/config.h>

class QDomDocument;
class QDomNode;
class QDomElement;
using namespace qutim_sdk_0_3;

namespace Core
{
	class PListConfigBackend : public qutim_sdk_0_3::ConfigBackend
	{
		Q_OBJECT
		Q_CLASSINFO("Extension", "plist")
	public:
		virtual QVariant load(const QString &file);
		virtual void save(const QString &file, const QVariant &entry);
	protected:
		QVariant generateConfigEntry (const QDomNode &val);
		QDomElement generateQDomElement (const QVariant &entry, QDomDocument &root);
	};
}
using namespace Core;

#endif // PLISTCONFIGBACKEND_H


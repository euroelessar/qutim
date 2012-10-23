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

#include "varianthook_p.h"
#include "localizedstring.h"
#include "extensionicon.h"
#include <QDebug>

Q_CORE_EXPORT const QVariant::Handler *qcoreVariantHandler();
static const QVariant::Handler *lastHandler = 0;

namespace Ureen
{
	static void construct(QVariant::Private *d, const void *copy)
	{
		switch (d->type) {
		default:
			lastHandler->construct(d, copy);
			return;
		}
//		d->is_null = !copy;
	}
	
	static void clear(QVariant::Private *d)
	{
		switch (d->type) {
		default:
			lastHandler->clear(d);
			return;
		}
//		d->type = QVariant::Invalid;
//		d->is_null = true;
//		d->is_shared = false;
	}
	
	
	static bool isNull(const QVariant::Private *d)
	{
		switch(d->type) {
		default:
			return lastHandler->isNull(d);
		}
		return d->is_null;
	}
	
	static bool compare(const QVariant::Private *a, const QVariant::Private *b)
	{
		Q_ASSERT(a->type == b->type);
		switch(a->type) {
		default:
			break;
		}
		return lastHandler->compare(a, b);
	}
	
	
	
	static bool convert(const QVariant::Private *d, QVariant::Type t,
					 void *result, bool *ok)
	{
		switch (t) {
		case QVariant::String: {
			QString *str = static_cast<QString *>(result);
			if (d->type == qMetaTypeId<LocalizedString>()) {
				*str = static_cast<LocalizedString*>(d->data.shared->ptr)->toString();
				return true;
			} else if (d->type == qMetaTypeId<LocalizedStringList>()) {
				LocalizedStringList *list = static_cast<LocalizedStringList*>(d->data.shared->ptr);
				if (list->size() == 1) {
					*str = list->at(0).toString();
					return true;
				} else {
					return false;
				}
			}
			break;
		}
		case QVariant::Icon: {
			QIcon *icon = static_cast<QIcon*>(result);
			if (d->type == qMetaTypeId<ExtensionIcon>()) {
				*icon = static_cast<ExtensionIcon*>(d->data.shared->ptr)->toIcon();
				return true;
			}
			break;
		}
		default:
			break;
		}
		return lastHandler->convert(d, t, result, ok);
	}
	
	static void streamDebug(QDebug dbg, const QVariant &v)
	{
		switch(v.type()) {
		default:
			lastHandler->debugStream(dbg, v);
			break;
		}
	}
	
	const QVariant::Handler VariantHook::handler = {
		Ureen::construct,
		Ureen::clear,
		Ureen::isNull,
		0,
		0,
		Ureen::compare,
		Ureen::convert,
		0,
		Ureen::streamDebug
	};
	
	void VariantHook::init()
	{
		lastHandler = QVariant::handler ? QVariant::handler : qcoreVariantHandler();
		QVariant::handler = &VariantHook::handler;
	}
}


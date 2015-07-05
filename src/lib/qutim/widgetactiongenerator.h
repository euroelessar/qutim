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

#ifndef WIDGETACTIONGENERATOR_H
#define WIDGETACTIONGENERATOR_H

#include "actiontoolbar.h"
#include <QWidgetAction>

namespace qutim_sdk_0_3
{
	struct WidgetActionGeneratorPrivate;

	class LIBQUTIM_EXPORT WidgetActionGenerator : public ActionGenerator
	{
	public:
		WidgetActionGenerator(const QIcon &icon, const LocalizedString &text, const QObject *receiver, const char *member);
		virtual ~WidgetActionGenerator();

		template <typename T>
		WidgetActionGenerator *setWidget()
		{ return setWidget(new GeneralGenerator<T>()); }
		WidgetActionGenerator *setWidget(ObjectGenerator *gen);
		WidgetActionGenerator *addWidgetProperty(const QByteArray &name, const QVariant &value);
	protected:
		virtual QObject *generateHelper() const;
	private:
		QScopedPointer<WidgetActionGeneratorPrivate> p;
	};
}

#endif // WIDGETACTIONGENERATOR_H


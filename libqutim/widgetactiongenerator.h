/****************************************************************************
 *  widgetactiongenerator.h
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

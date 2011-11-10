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

#include "widgetactiongenerator.h"

namespace qutim_sdk_0_3
{
	struct WidgetActionGeneratorPrivate
	{
		QList<QByteArray> names;
		QList<QVariant>   values;
		ObjectGenerator *generator;
	};

	WidgetActionGenerator::WidgetActionGenerator(const QIcon &icon, const LocalizedString &text, const QObject *receiver, const char *member)
			: ActionGenerator(icon, text, receiver, member)
	{
		p->generator = 0;
	}

	WidgetActionGenerator::~WidgetActionGenerator()
	{
	}

	WidgetActionGenerator *WidgetActionGenerator::setWidget(ObjectGenerator *gen)
	{
		Q_ASSERT(gen);
		if (p->generator)
			delete p->generator;
		p->generator = gen;
		return this;
	}

	WidgetActionGenerator *WidgetActionGenerator::addWidgetProperty(const QByteArray &name, const QVariant &value)
	{
		int index = p->names.indexOf(name);
		if (index != -1) {
			p->values[index] = value;
		} else {
			p->names.append(name);
			p->values.append(value);
		}
		return this;
	}

	QObject *WidgetActionGenerator::generateHelper() const
	{
		QAction *action = prepareAction(new QWidgetAction(NULL));
		if (p->generator && action) {
			QWidget *widget = p->generator->generate<QWidget>();
			for (int i = 0; i < p->names.size(); i++)
				widget->setProperty(p->names.at(i), p->values.at(i));
			static_cast<QWidgetAction *>(action)->setDefaultWidget(widget);
		}
		return action;
	}
}


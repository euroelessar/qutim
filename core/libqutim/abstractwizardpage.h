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

#ifndef ABSTRACTWIZARDPAGE_H
#define ABSTRACTWIZARDPAGE_H

#include "libqutim_global.h"

namespace qutim_sdk_0_3
{
	class LIBQUTIM_EXPORT AbstractWizardPage : public QObject
	{
		Q_OBJECT
	public:
		enum WidgetType { SeparateWindow, WizardPage };
		virtual QWidget *widget() = 0;
		virtual WidgetType widgetType() { return WizardPage; }
		virtual bool isComplete() = 0;
		virtual QString title() = 0;
		virtual QString subTitle() = 0;
	signals:
		void completeChanged();
	protected:
		AbstractWizardPage();
		virtual void virtual_hook(int id, void *data);
		virtual ~AbstractWizardPage();
	};
}

#endif // ABSTRACTWIZARDPAGE_H


/****************************************************************************
 *  abstractwizardpage.h
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
		virtual ~AbstractWizardPage();
	};
}

#endif // ABSTRACTWIZARDPAGE_H

/****************************************************************************
 *  profilecreatorpage.h
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

#ifndef PROFILECREATORPAGE_H
#define PROFILECREATORPAGE_H

#include "extensioninfo.h"
#include <QWizardPage>

namespace qutim_sdk_0_3
{
	class LIBQUTIM_EXPORT ProfileCreatorPage : public QObject
	{
		Q_OBJECT
	public:
		ProfileCreatorPage();
		virtual ~ProfileCreatorPage();
		virtual ExtensionInfo info() const;
		// Creators with higher priority are shown earlier, default is 0.0
		virtual double priority() const;
		virtual QList<QWizardPage *> pages(QWidget *parent) = 0;
	protected:
		virtual void virtual_hook(int id, void *data);
	};
}

#endif // PROFILECREATORPAGE_H

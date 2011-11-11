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


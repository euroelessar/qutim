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
#ifndef ACCOUNTCREATOR_H
#define ACCOUNTCREATOR_H

#include <qutim/profilecreatorpage.h>
#include <qutim/plugin.h>

using namespace qutim_sdk_0_3;

namespace Core
{
class AccountCreator : public Plugin
{
	Q_OBJECT
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
	virtual ~AccountCreator();
private slots:
	void showWizard();
};

class AccountPageCreator : public ProfileCreatorPage
{
	Q_OBJECT
public:
	explicit AccountPageCreator(QObject *parent = 0);
	virtual double priority() const;
	virtual QList<QWizardPage *> pages(QWidget *parent);
};
}

#endif // ACCOUNTCREATOR_H


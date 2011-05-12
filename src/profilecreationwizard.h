/****************************************************************************
**
** qutIM instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@ya.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
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

#ifndef PROFILECREATIONWIZARD_H
#define PROFILECREATIONWIZARD_H

#include <QWizard>
#include <qutim/modulemanager.h>

namespace Core
{
using namespace qutim_sdk_0_3;
class ProfileCreationWizard : public QWizard
{
	Q_OBJECT
public:
	explicit ProfileCreationWizard(ModuleManager *parent,
								   const QString &id, const QString &password,
								   bool singleProfile = false);
protected:
    void done(int result);
private:
	ModuleManager *m_manager;
	bool m_singleProfile;
};
}

#endif // PROFILECREATIONWIZARD_H

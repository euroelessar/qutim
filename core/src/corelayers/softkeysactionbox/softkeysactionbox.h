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
#ifndef SOFTKEYSACTIONBOX_H
#define SOFTKEYSACTIONBOX_H

#include <qutim/startupmodule.h>
#include <qutim/actionbox.h>
#include <QMap>

class QToolButton;
class QHBoxLayout;

namespace Core
{
using namespace qutim_sdk_0_3;

class SoftkeysActionBoxModule : public ActionBoxModule
{
    Q_OBJECT
	Q_CLASSINFO("DebugName","SoftkeysActionBox")
public:
	explicit SoftkeysActionBoxModule();
	virtual void addAction(QAction *action);
	virtual void removeAction(QAction *action);
};

class SoftkeysActionBoxGenerator : public ActionBoxGenerator
{
	Q_OBJECT
public:
	virtual ActionBoxModule* generate() {return new SoftkeysActionBoxModule(); }
};

}
#endif // SOFTKEYSACTIONBOX_H


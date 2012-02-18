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
#ifndef SIMPLEACTIONBOX_H
#define SIMPLEACTIONBOX_H

#include <qutim/startupmodule.h>
#include <qutim/actionbox.h>
#include <QMap>

class QToolButton;
class QHBoxLayout;
namespace Core
{
using namespace qutim_sdk_0_3;

class SimpleActionBoxModule : public ActionBoxModule
{
    Q_OBJECT
	Q_CLASSINFO("DebugName","SimpleActionBox")
public:
	explicit SimpleActionBoxModule();
	virtual void addAction(QAction *action);
	virtual void removeAction(QAction *action);
protected slots:
	void onButtonDestroyed(QObject *obj);
	void onChanged();
private:
	QHBoxLayout *m_layout;
	QMap<QAction*,QToolButton*> m_buttons;
};

class SimpleActionBoxGenerator : public ActionBoxGenerator
{
	Q_OBJECT
public:
	virtual ActionBoxModule* generate() {return new SimpleActionBoxModule(); }
};

}
#endif // SIMPLEACTIONBOX_H


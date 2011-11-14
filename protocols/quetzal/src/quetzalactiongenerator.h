/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#ifndef QUETZALACTIONGENERATOR_H
#define QUETZALACTIONGENERATOR_H

#include <qutim/actiongenerator.h>
#include <QMetaType>
#include <purple.h>

using namespace qutim_sdk_0_3;
struct QuetzalActionInfo;

class QuetzalActionGenerator : public ActionGenerator
{
public:
	QuetzalActionGenerator(PurpleMenuAction *action, void *node);
	QuetzalActionGenerator(PurplePluginAction *action);
protected:
	QObject *generateHelper() const;
private:
	QScopedPointer<QuetzalActionInfo> m_info;
	QSharedPointer<PurplePluginAction> m_action;
};

Q_DECLARE_METATYPE(QSharedPointer<PurplePluginAction>)

#endif // QUETZALACTIONGENERATOR_H


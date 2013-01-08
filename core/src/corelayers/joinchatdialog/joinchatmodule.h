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

#ifndef JOINCHATMODULE_H
#define JOINCHATMODULE_H

#include "joinchatdialog.h"
#include <qutim/actiongenerator.h>

namespace Core
{
class JoinChatGenerator;

class JoinChatModule : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "JoinGroupChat")
	Q_CLASSINFO("Uses", "IconLoader")
	Q_CLASSINFO("Uses", "ContactList")
public:
	JoinChatModule();
	virtual ~JoinChatModule();

public slots:
	void onJoinChatTriggered();

private:
	QScopedPointer<JoinChatGenerator> m_action;
	QPointer<JoinChatDialog> m_dialog;
};

class JoinChatGenerator : public qutim_sdk_0_3::ActionGenerator
{
public:
	JoinChatGenerator(QObject *module);
	void showImpl(QAction *action, QObject *obj);
};
}

#endif // JOINCHATMODULE_H


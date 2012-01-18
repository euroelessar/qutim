/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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

#ifndef SIMPLEPLUGIN_H
#define SIMPLEPLUGIN_H
#include <qutim/plugin.h>
#include <QAction>

namespace MassMessaging
{

using namespace qutim_sdk_0_3;


class Manager;
class MessagingDialog;

class MassMessaging : public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "MassMessaging")
	Q_CLASSINFO("Uses", "ContactList")
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
private slots:
	void onActionTriggered();
private:
	QPointer<MessagingDialog> m_dialog;
};

}
#endif


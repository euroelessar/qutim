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


#ifndef SESSIONHELPER_PLUGIN_H
#define SESSIONHELPER_PLUGIN_H
#include <qutim/plugin.h>
#include <qutim/chatsession.h>

namespace qutim_sdk_0_3 {
class ChatSession;
class Message;
}

namespace SessionHelper {

using namespace qutim_sdk_0_3;

class SessionHelper : public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("DebugName", "SessionHelper")
	Q_CLASSINFO("Uses", "ChatLayer")
public:
	virtual void init();
	virtual bool load();
	virtual bool unload();
private slots:
	void sessionCreated(qutim_sdk_0_3::ChatSession*);
private:
};

} //SessionHelper
#endif //SESSIONHELPER_PLUGIN_H


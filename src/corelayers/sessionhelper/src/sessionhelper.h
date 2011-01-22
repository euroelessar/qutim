/*
    sessionhelper.h

  (c) 2011 by Aleksey Sidorov <sauron@citadelspb.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/


#ifndef SESSIONHELPER_PLUGIN_H
#define SESSIONHELPER_PLUGIN_H
#include <qutim/plugin.h>
#include <qutim/messagesession.h>

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

/****************************************************************************
 *  messaging.h
 *
 *  Copyright (c) 2010-2011 by Sidorov Aleksey <sauron@citadelspb.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*****************************************************************************/

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

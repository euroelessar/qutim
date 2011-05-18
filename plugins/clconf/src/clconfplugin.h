/****************************************************************************
 *  clconfplugin.h
 *
 *  Copyright (c) 2010 by Prokhin Alexey <alexey.prokhin@yandex.ru>
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

#ifndef CLCONFPLUGIN_H
#define CLCONFPLUGIN_H

#include "proxyaccount.h"
#include "proxycontact.h"
#include <qutim/plugin.h>
#include <QSet>

class ClConfPlugin : public Plugin
{
	Q_OBJECT
	Q_CLASSINFO("Service", "ConferenceManager")
	Q_CLASSINFO("Uses", "ChatLayer")
public:
	ClConfPlugin();
	virtual void init();
	virtual bool load();
	virtual bool unload();
	static ClConfPlugin *instance() { Q_ASSERT(self); return self; }
	ProxyAccount *account(Account *account) { return m_accounts.value(account); }
private slots:
	void onAccountCreated(qutim_sdk_0_3::Account *account);
	void onAccountDestroyed(QObject *account);
	void onConferenceCreated(qutim_sdk_0_3::Conference *conference);
private:
	static ClConfPlugin *self;
	QHash<QObject *, ProxyAccount *> m_accounts;
};

#endif // CLCONFPLUGIN_H

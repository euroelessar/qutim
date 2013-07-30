/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Alexey Prokhin <alexey.prokhin@yandex.ru>
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

#ifndef CLCONFPLUGIN_H
#define CLCONFPLUGIN_H

#include "proxyaccount.h"
#include "proxycontact.h"
#include <qutim/plugin.h>
#include <QSet>

class ClConfPlugin : public Plugin
{
	Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qutim.Plugin")
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


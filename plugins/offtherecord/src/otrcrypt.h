/*

    Copyright (c) 2010 by Stanislav (proDOOMman) Kosolapov <prodoomman@shell.tor.hu>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#ifndef OTRCRYPT_H
#define OTRCRYPT_H

#include "global.h"
#include "otrinternal.h"
#include "qutimotrclosure.h"
#include "otrmessaging.h"
#include "qutimotrclosure.h"
#include "otrsettingswidget.h"
#include "otrmessagehandler.h"
#include <qutim/plugin.h>
#include <qutim/actiongenerator.h>
#include <qutim/settingslayer.h>
#include <QHash>
#include <QSystemSemaphore>

using namespace OtrSupport;

class OtrActionGenerator : public qutim_sdk_0_3::ActionGenerator
{
public:
	OtrActionGenerator();
	void createImpl(QAction *action, QObject *obj) const;
	void showImpl(QAction *action,QObject *obj);
};

class OTRCrypt : public qutim_sdk_0_3::Plugin
{
    Q_OBJECT
	Q_CLASSINFO("DebugName", "OTR")
public:
	OTRCrypt();
	~OTRCrypt();
	
	virtual void init();
    virtual bool load();
    virtual bool unload();
	
	static OTRCrypt *instance();
	
	OtrClosure *ensureClosure(qutim_sdk_0_3::ChatUnit *unit);
	OtrClosure *getClosure(qutim_sdk_0_3::ChatUnit *unit);
	OtrMessaging *connectionForPolicy(int policy);
	bool notifyUser() const;
	void updateAction(OtrClosure *closure);
	void disableAccount(qutim_sdk_0_3::Account *account);
	void enableAccount(qutim_sdk_0_3::Account *account);
	bool isEnabledAccount(qutim_sdk_0_3::Account *account);
	
signals:

public slots:
	void loadSettings();
	void onActionTriggered(QAction *action);

private:
	QScopedPointer<OtrActionGenerator> m_action;
	QScopedPointer<OtrMessagePreHandler> m_preHandler;
	QScopedPointer<OtrMessagePostHandler> m_postHandler;
	QScopedPointer<qutim_sdk_0_3::SettingsItem> m_settingsItem;
	QHash<qutim_sdk_0_3::ChatUnit*, OtrClosure*> m_closures;
	QSet<qutim_sdk_0_3::Account*> m_disabledAccounts;
	QList<OtrMessaging*> m_connections;
	OtrlUserState m_state;
	bool m_notify;
};

#endif // OTRCRYPT_H

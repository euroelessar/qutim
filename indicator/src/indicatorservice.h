/****************************************************************************
 * indicatorservice.h
 *  Copyright © 2010-2011, Vsevolod Velichko <torkvema@gmail.com>.
 *  Licence: GPLv3 or later
 *
 ****************************************************************************
 *                                                                          *
 *   This library is free software; you can redistribute it and/or modify   *
 *   it under the terms of the GNU General Public License as published by   *
 *   the Free Software Foundation; either version 3 of the License, or      *
 *   (at your option) any later version.                                    *
 *                                                                          *
 ****************************************************************************/
#ifndef INDICATORSERVICE_23DW9H42
#define INDICATORSERVICE_23DW9H42

#define QUTIM_DESKTOP_FILE DESKTOPDIR "/qutim.desktop"

#include <qindicateindicator.h>
#include <qindicateserver.h>

#include <qutim/chatsession.h>
#include <qutim/account.h>
#include <qutim/protocol.h>

typedef QHash<qutim_sdk_0_3::ChatSession*, QIndicate::Indicator*> HashIndicator;

class IndicatorService : public qutim_sdk_0_3::MenuController
{
	Q_OBJECT
	Q_CLASSINFO("Service", "TrayIcon")
	Q_CLASSINFO("Uses", "ContactList")
	Q_CLASSINFO("Uses", "ChatLayer")
	Q_CLASSINFO("Uses", "IconLoader")
public:
	explicit IndicatorService();
	virtual ~IndicatorService();
protected slots:
	/* Tray layer slots */
	void onSessionCreated(qutim_sdk_0_3::ChatSession*);
	void onSessionDestroyed(QObject* session);
	void onUnreadChanged(const qutim_sdk_0_3::MessageList&);
	////// void onActivated(QSystemTrayIcon::ActivationReason);
	void onAccountCreated(qutim_sdk_0_3::Account *);
	void onAccountDestroyed(QObject *obj);
	////// void onStatusChanged(const qutim_sdk_0_3::Status &);
	void loadSettings();
	/* Indicator stuff */
	void onSessionActivated(bool);
	void onIndicatorDisplay(QIndicate::Indicator*);
	void showMainWindow();
private:
	HashIndicator sessionIndicators;
	QString desktopName;
	QIndicate::Server* indicateServer;
	QIndicate::Indicator *quitButton;
};

#endif /* end of include guard: INDICATORSERVICE_23DW9H42 */

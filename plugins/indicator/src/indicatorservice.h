/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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
#ifndef INDICATORSERVICE_23DW9H42
#define INDICATORSERVICE_23DW9H42

#ifndef DESKTOPDIR
# define DESKTOPDIR ""
#endif
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


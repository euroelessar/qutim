/****************************************************************************
 * indicator.h
 *  Copyright © 2010, Vsevolod Velichko <torkvema@gmail.com>.
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

#ifndef INDICATOR_GS5D2FLA

#define INDICATOR_GS5D2FLA

#define QUTIM_DESKTOP_FILE DESKTOPDIR "/qutim.desktop"
	
#include <qindicateindicator.h>
#include <qindicateserver.h>
#include <qutim/plugin.h>
#include <qutim/messagesession.h>

typedef QHash<qutim_sdk_0_3::ChatSession*, QIndicate::Indicator*> HashIndicator;

class IndicatorPlugin : public qutim_sdk_0_3::Plugin
{
	Q_OBJECT
	public:
		explicit IndicatorPlugin ();
		virtual void init();
		virtual bool load();
		virtual bool unload();
	protected slots:
		void onSessionCreated(qutim_sdk_0_3::ChatSession*);
		void onSessionDestroyed(QObject*);
		void onSessionActivated(bool);
		void onUnreadChanged(const qutim_sdk_0_3::MessageList&);
		void onIndicatorDisplay(QIndicate::Indicator*);
		void showMainWindow();
	private:
		QIndicate::Server* indicateServer;
		HashIndicator sessionIndicators;
		QString desktopName;
};

#endif /* end of include guard: INDICATOR_GS5D2FLA */

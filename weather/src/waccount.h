/****************************************************************************
 * waccount.h
 *
 *  Copyright (c) 2010 by Belov Nikita <null@deltaz.org>
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

#ifndef WACCOUNT_H
#define WACCOUNT_H

#include <qutim/account.h>
#include <qutim/settingslayer.h>

#include "wcontact.h"
#include "wsettings.h"

using namespace qutim_sdk_0_3;

class WProtocol;

class WAccount : public Account
{
	Q_OBJECT

public:
	WAccount( WProtocol *protocol );
	virtual ~WAccount();

	ChatUnit *getUnitForSession( ChatUnit *unit );
	ChatUnit *getUnit( const QString &unitId, bool create = false );

	QString name() const;

	// settings
	QString getThemePath();
	bool getShowStatusRow();

	virtual void setStatus( Status status );

private slots:
	void loadSettings();
	void updateAll();

private:
	void loadContacts();

	QHash< QString, WContact * > m_contacts;

	QTimer *m_timer;

	// settings
	bool s_showStatusRow;
	QString s_themePath;
};

#endif // WACCOUNT_H
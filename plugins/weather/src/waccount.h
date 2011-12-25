/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Belov Nikita <null@deltaz.org>
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

	SettingsItem *settings;

	QHash< QString, WContact * > m_contacts;

	QTimer *m_timer;

	// settings
	bool s_showStatusRow;
	QString s_themePath;
};

#endif // WACCOUNT_H


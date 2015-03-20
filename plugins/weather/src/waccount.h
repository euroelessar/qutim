/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Nikita Belov <null@deltaz.org>
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

#include "wcontact.h"
#include "wsettings.h"
#include <qutim/account.h>
#include <qutim/settingslayer.h>
#include <QNetworkAccessManager>

using namespace qutim_sdk_0_3;

class WProtocol;
class WContact;

class WAccount : public Account
{
	Q_OBJECT

public:
	WAccount(WProtocol *protocol);
	virtual ~WAccount();

	ChatUnit *getUnitForSession(ChatUnit *unit);
	ChatUnit *getUnit(const QString &unitId, bool create = false);

	QString name() const;

	// settings
	QString getThemePath();
	bool getShowStatusRow();

	void doConnectToServer() override;
	void doDisconnectFromServer() override;
	void doStatusChange(const Status &status) override;

	void update(WContact *contact, bool needMessage);
	void getForecast(WContact *contact);
	
	void timerEvent(QTimerEvent *event);

private slots:
	void loadSettings();
	void onNetworkReply(QNetworkReply *reply);

private:
	void fillStrings(QString &text, QString &html, const QDomElement &element, const QString &prefix);
	QString loadResourceFile(const QString &fileName);
	void loadContacts();

	SettingsItem *m_settings;
	QHash< QString, WContact * > m_contacts;
	QBasicTimer m_timer;
	QNetworkAccessManager m_manager;

	// settings
	bool m_showStatusRow;
	QString m_themePath;
};

#endif // WACCOUNT_H


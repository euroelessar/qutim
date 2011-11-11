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

#ifndef JSOFTWAREDETECTION_H
#define JSOFTWAREDETECTION_H

#include <QObject>
#include <QSet>
#include <QBasicTimer>
#include <QStringList>
#include "sdk/jabber.h"
#include <jreen/disco.h>

namespace qutim_sdk_0_3 {
class ChatUnit;
}

namespace Jreen
{
class Presence;
class IQ;
class Disco;
}

namespace Jabber
{
class JAccount;
class JContactResource;

class JSoftwareDetection : public QObject
{
	enum { RequestDisco, RequestSoftware };
	Q_OBJECT
public:
	struct SoftwareInfo
	{
		SoftwareInfo() : finished(false) {}
		QSet<QString> features;
		QString name;
		QString version;
		QString os;
		QString icon;
		QString description;
		bool finished;
	};
	typedef QHash<QString, SoftwareInfo> SoftwareInfoHash;

	JSoftwareDetection(JAccount *account);
	~JSoftwareDetection();
	
protected:
	void timerEvent(QTimerEvent *ev);
protected slots:
	void handlePresence(const Jreen::Presence &presence);
	void onSoftwareRequestFinished(const Jreen::IQ &iq);
	void onInfoRequestFinished();
private:
	void requestSoftware(const Jreen::JID &jid);
	void updateCache(const QString &node, const SoftwareInfo &info, bool fixed = false);
	void updateClientData(JContactResource *resource, const QString &client,
						  const QString &software, const QString &softwareVersion,
						  const QString &os, const QString &clientIcon);
	void setClientInfo(JContactResource *resource, const QString &client, const QString &clientIcon);
	QString getClientDescription(const QString &software, const QString &softwareVersion, const QString &os);
	QString getClientIcon(const QString &software);
private:
	JAccount *m_account;
	QHash<QString, SoftwareInfo> m_hash;
	QStringList m_recent;
	QBasicTimer m_timer;
};
}

#endif // JSOFTWAREDETECTION_H


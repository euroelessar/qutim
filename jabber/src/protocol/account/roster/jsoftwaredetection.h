/****************************************************************************
 *  jsoftwaredetection.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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

#ifndef JSOFTWAREDETECTION_H
#define JSOFTWAREDETECTION_H

#include <QObject>
#include <QSet>
#include "sdk/jabber.h"

namespace qutim_sdk_0_3 {
class ChatUnit;
}

namespace jreen
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
		QSet<QString> features;
		QString name;
		QString version;
		QString os;
		QString icon;
		QString description;
	};
	typedef QHash<QString, SoftwareInfo> SoftwareInfoHash;

	JSoftwareDetection(JAccount *account);
	~JSoftwareDetection();
protected slots:
	void handlePresence(const jreen::Presence &presence);
	void handleIQ(const jreen::IQ &iq);
	void handleIQ(const jreen::IQ &iq, int context);
private:
	void updateClientData(JContactResource *resource, const QString &client,
						  const QString &software, const QString &softwareVersion,
						  const QString &os, const QString &clientIcon);
	void setClientInfo(JContactResource *resource, const QString &client, const QString &clientIcon);
	QString getClientDescription(const QString &software, const QString &softwareVersion, const QString &os);
	QString getClientIcon(const QString &software);
private:
	JAccount *m_account;
	QHash<QString, SoftwareInfo> m_hash;
};
}

#endif // JSOFTWAREDETECTION_H

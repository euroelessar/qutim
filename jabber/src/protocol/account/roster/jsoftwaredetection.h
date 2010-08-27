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
#include <gloox/softwareversion.h>
#include <gloox/client.h>
#include <gloox/presencehandler.h>
#include <gloox/discohandler.h>
#include "sdk/jabber.h"

namespace qutim_sdk_0_3 {
	class ChatUnit;
}

namespace Jabber
{
	class JAccount;

	class JSoftwareDetection :
			public QObject,
			public gloox::IqHandler,
			public gloox::PresenceHandler,
			public gloox::DiscoHandler
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

		JSoftwareDetection(JAccount *account, const JabberParams &params);
		~JSoftwareDetection();

		void handlePresence(const gloox::Presence &presence);
		bool handleIq(const gloox::IQ &iq);
		void handleIqID(const gloox::IQ &iq, int context);
		void handleDiscoInfo(const gloox::JID &from, const gloox::Disco::Info &info, int context);
		void handleDiscoItems(const gloox::JID &from, const gloox::Disco::Items &items, int context);
		void handleDiscoError(const gloox::JID &from, const gloox::Error *error, int context);
	private:
		void updateClientData(qutim_sdk_0_3::ChatUnit *unit, const QString &client,
							  const QString &software, const QString &softwareVersion,
							  const QString &os, const QString &clientIcon);
		QString getClientDescription(const QString &software, const QString &softwareVersion, const QString &os);
		QString getClientIcon(const QString &software);
	private:
		JAccount *m_account;
		QHash<QString, SoftwareInfo> m_hash;
	};
}

#endif // JSOFTWAREDETECTION_H

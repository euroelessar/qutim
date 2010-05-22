/****************************************************************************
 *  jsoftwaredetection.cpp
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

#include "jsoftwaredetection.h"
#include "../jaccount.h"
#include "jcontactresource.h"
#include <gloox/dataform.h>
#include <gloox/capabilities.h>

using namespace qutim_sdk_0_3;
using namespace gloox;

namespace Jabber
{
	JSoftwareDetection::JSoftwareDetection(JAccount *account, const JabberParams &params) : QObject(account)
	{
		m_account = account;
		params.item<Client>()->registerPresenceHandler(this);
	}

	JSoftwareDetection::~JSoftwareDetection()
	{
	}

	void JSoftwareDetection::handlePresence(const Presence &presence)
	{
		QString jid = QString::fromStdString(presence.from().full());
		ChatUnit *unit = m_account->getUnit(jid, false);

		if (JContactResource *resource = qobject_cast<JContactResource *>(unit)) {
			if (!resource->features().isEmpty())
				return;

			std::string node;

			if (const Capabilities *caps = presence.findExtension<Capabilities>(ExtCaps)) {
				node = caps->node() + '#' + caps->ver();
				QString qNode = QString::fromStdString(node);
				unit->setProperty("node", qNode);
				SoftwareInfoHash::iterator it = m_hash.find(qNode);
				if (it != m_hash.end()) {
					SoftwareInfo &info = *it;
					resource->setFeatures(info.features);
					if (info.name.isEmpty()) {
						IQ iq(IQ::Get, presence.from());
						iq.addExtension(new SoftwareVersion());
						m_account->client()->send(iq, this, RequestSoftware);
					} else {
						unit->setProperty("client", (info.name + " " + info.version).trimmed());
						unit->setProperty("os", info.os);
					}
					return;
				}
			}

			m_account->client()->disco()->getDiscoInfo(presence.from(), node, this, RequestDisco);
		}
	}

	bool JSoftwareDetection::handleIq(const IQ &iq)
	{
		Q_UNUSED(iq);
		return false;
	}

	void JSoftwareDetection::handleIqID(const IQ &iq, int context)
	{
		if (context == RequestSoftware) {
			if (const SoftwareVersion *soft = iq.findExtension<SoftwareVersion>(ExtVersion)) {
				if (ChatUnit *unit = m_account->getUnit(QString::fromStdString(iq.from().full()), false)) {
					QString node = unit->property("node").toString();
					QString software = QString::fromStdString(soft->name());
					QString softwareVersion = QString::fromStdString(soft->version());
					QString os = QString::fromStdString(soft->os());
					unit->setProperty("client", (software + " " + softwareVersion).trimmed());
					unit->setProperty("software", software);
					unit->setProperty("softwareVersion", softwareVersion);
					unit->setProperty("os", os);
					SoftwareInfoHash::iterator it = m_hash.find(node);
					if (it != m_hash.end()) {
						SoftwareInfo &info = (*it);
						info.name = software;
						info.version = softwareVersion;
						info.os = os;
						info.icon = info.name.toLower();
					}
				}
			}
		}
	}

	void JSoftwareDetection::handleDiscoInfo(const JID &from, const Disco::Info &discoInfo, int context)
	{
		Q_ASSERT(context == RequestDisco);

		QString node = QString::fromStdString(discoInfo.node());

		SoftwareInfo info;
		foreach(const std::string &str, discoInfo.features())
			info.features << QString::fromStdString(str);

		QString jid = QString::fromStdString(from.full());
		const DataForm *form = discoInfo.form();

		if (DataFormField *field = (form ? form->field("FORM_TYPE") : 0)) {
			if (field->value() == "urn:xmpp:dataforms:softwareinfo") {
				QString software;
				QString softwareVersion;
				QString os;
				QString osVersion;
				QString softwareIcon;
				foreach(DataFormField *field, form->fields()) {
					if (field->name() == "os") {
						os = QString::fromStdString(field->value());
					} else if (field->name() == "os_version") {
						osVersion = QString::fromStdString(field->value());
					} else if (field->name() == "software") {
						software = QString::fromStdString(field->value());
					} else if (field->name() == "software_version") {
						softwareVersion = QString::fromStdString(field->value());
					}
				}
				if (!software.isEmpty()) {
					info.icon = software.toLower();
					info.name = software;
					if (!softwareVersion.isEmpty())
						info.version = softwareVersion;
				}
				if (!os.isEmpty()) {
					info.os = os;
					if (!osVersion.isEmpty())
						info.os.append(" ").append(osVersion);
				}
			}
		}

		m_hash.insert(node, info);

		if (JContactResource *unit = qobject_cast<JContactResource*>(m_account->getUnit(jid, false))) {
			if (unit->property("node").isNull())
				unit->setProperty("node", node);

			if (info.name.isEmpty()) {
				IQ iq(IQ::Get, from);
				iq.addExtension(new SoftwareVersion());
				m_account->client()->send(iq, this, RequestSoftware);
			}
			unit->setFeatures(info.features);
		}
	}

	void JSoftwareDetection::handleDiscoItems(const JID &from, const Disco::Items &items, int context)
	{
		Q_UNUSED(from);
		Q_UNUSED(items);
		Q_UNUSED(context);
	}

	void JSoftwareDetection::handleDiscoError(const JID &from, const Error *error, int context)
	{
		Q_UNUSED(from);
		Q_UNUSED(error);
		Q_UNUSED(context);
	}
}

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
#include <qutim/extensionicon.h>

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
				static const QRegExp regExp("^http://.*google.com/.*client/caps$");
				Q_ASSERT(regExp.isValid());
				QString capsNode = QString::fromStdString(caps->node());
				if(regExp.exactMatch(capsNode))
				{
					QString software = "GTalk";
					if(capsNode.startsWith("http://mail."))
						software += " (GMail)";
					else if(capsNode.startsWith("http://talkgadget."))
						software += " (Gadget)";
					QString softwareVersion = QString::fromStdString(caps->ver());
					QString client = getClientDescription(software, softwareVersion, QString());
					updateClientData(resource, client, software, softwareVersion, QString(), "gtalk");
					return;
				} else {
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
							updateClientData(resource, info.description, info.name, info.version, info.os, info.icon);
						}
						return;
					}
				}
			}

			//setClientInfo(resource, "", "unknown-client");
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
				ChatUnit *unit = m_account->getUnit(QString::fromStdString(iq.from().full()), false);
				if (JContactResource *resource = qobject_cast<JContactResource*>(unit)) {
					QString node = resource->property("node").toString();
					QString software = QString::fromStdString(soft->name());
					QString softwareVersion = QString::fromStdString(soft->version());
					QString os = QString::fromStdString(soft->os());
					QString icon = getClientIcon(software);;
					QString client = getClientDescription(software, softwareVersion, os);
					updateClientData(resource, client, software, softwareVersion, os, icon);
					SoftwareInfoHash::iterator it = m_hash.find(node);
					if (it != m_hash.end()) {
						SoftwareInfo &info = (*it);
						info.name = software;
						info.version = softwareVersion;
						info.os = os;
						info.icon = icon;
						info.description = client;
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
				QString icon = getClientIcon(software);
				QString client = getClientDescription(software, softwareVersion, os);

				if (!software.isEmpty()) {
					info.icon = icon;
					info.name = software;
					if (!softwareVersion.isEmpty())
						info.version = softwareVersion;
				}
				if (!os.isEmpty()) {
					info.os = os;
					if (!osVersion.isEmpty())
						info.os.append(" ").append(osVersion);
				}
				info.description = client;
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
			} else {
				updateClientData(unit, info.description, info.name, info.version, info.os, info.icon);
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

	void JSoftwareDetection::updateClientData(JContactResource *resource, const QString &client,
											  const QString &software, const QString &softwareVersion,
											  const QString &os, const QString &icon)
	{
		resource->setProperty("client", client);
		resource->setProperty("software", software);
		resource->setProperty("softwareVersion", softwareVersion);
		resource->setProperty("os", os);
		resource->setProperty("clientIcon", icon);
		setClientInfo(resource, client, icon);
	}

	void JSoftwareDetection::setClientInfo(JContactResource *resource, const QString &client, const QString &icon)
	{
		QVariantHash clientInfo;
		ExtensionIcon extIcon(icon);
		clientInfo.insert("id", "client");
		clientInfo.insert("title", tr("Possible client"));
		clientInfo.insert("icon", QVariant::fromValue(extIcon));
		clientInfo.insert("description", client);
		clientInfo.insert("priority", 85);
		resource->setExtendedInfo("client", clientInfo);
	}

	QString JSoftwareDetection::getClientDescription(const QString &software, const QString &softwareVersion,
													 const QString &os)
	{
		QString desc = software;
		if (!softwareVersion.isEmpty())
			desc += " " + softwareVersion;
		return desc;
	}

	QString JSoftwareDetection::getClientIcon(const QString &software)
	{
		if (software.isEmpty())
			return QString();
		if (software == "Miranda IM Jabber")
			return "miranda-jabber";
		else if (software == "bombusmod")
			return "bombus-mod";
		else if (software == "bombusqd")
			return "bombus-qd";
		else if (software == "bombus.pl")
			return "bombus-pl";
		else if (software == "bombus+")
			return "bombus-p";
		else if (software == "я.онлайн")
			return "yachat";
		else if (software == "hotcoffee")
			return "miranda-hotcoffee";
		else if (software == "jabber.el")
			return "emacs";
		else if (software == "just another jabber client")
			return "jajc";
		else if (software == "пиджин") // Stupid pidgin devels! Name mustn't be localized!
			return "pidgin";
		return software.toLower().replace(' ', '-') + "-jabber";
	}

}

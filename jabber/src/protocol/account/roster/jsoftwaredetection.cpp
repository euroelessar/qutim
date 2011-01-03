/****************************************************************************
 *  jsoftwaredetection.cpp
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *  Copyright (c) 2010 by Sidorov Aleksey <sauron@citadelspb.com>
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
#include <qutim/extensionicon.h>
#include <jreen/client.h>
#include <jreen/disco.h>
#include <jreen/dataform.h>
#include <jreen/softwareversion.h>
#include <jreen/capabilities.h>
#include <jreen/iq.h>
#include <qutim/debug.h>
#include <qutim/json.h>

using namespace qutim_sdk_0_3;
using namespace gloox;

namespace Jabber
{
JSoftwareDetection::JSoftwareDetection(JAccount *account) : QObject(account)
{
	m_account = account;
	jreen::Client *client = account->client();
	connect(client,SIGNAL(newPresence(jreen::Presence)),SLOT(handlePresence(jreen::Presence)));
}

JSoftwareDetection::~JSoftwareDetection()
{
}

void JSoftwareDetection::handlePresence(const jreen::Presence &presence)
{
	QString jid = presence.from().full();
	ChatUnit *unit = m_account->getUnit(jid, false);

	if (JContactResource *resource = qobject_cast<JContactResource *>(unit)) {
		if (!resource->features().isEmpty())
			return;

		QString node;
		if (jreen::Capabilities *caps = presence.findExtension<jreen::Capabilities>().data()) {
			qDebug() << "handle caps" << caps->node();
			static const QRegExp regExp("^http://.*google.com/.*client/caps$");
			Q_ASSERT(regExp.isValid());
			QString capsNode = caps->node();
			if(regExp.exactMatch(capsNode))	{
				QString software = "GTalk";
				if(capsNode.startsWith("http://mail."))
					software += " (GMail)";
				else if(capsNode.startsWith("http://talkgadget."))
					software += " (Gadget)";
				QString softwareVersion = caps->ver();
				QString client = getClientDescription(software, softwareVersion, QString());
				updateClientData(resource, client, software, softwareVersion, QString(), "gtalk");
				return;
			} else {
				node = caps->node() + '#' + caps->ver();
				QString qNode = node;
				unit->setProperty("node", qNode);
				SoftwareInfoHash::iterator it = m_hash.find(qNode);
				qDebug() << "find from hash" << m_hash.count();
				if (it != m_hash.end()) {
					SoftwareInfo &info = *it;
					resource->setFeatures(info.features);
					qDebug() << info.name;
					if (info.name.isEmpty()) {
						qDebug() << "Send software version request";
						jreen::IQ iq(jreen::IQ::Get, presence.from());
						iq.addExtension(new jreen::SoftwareVersion());
						m_account->client()->send(iq,this,SLOT(handleIQ(jreen::IQ,int)),RequestSoftware);
					} else {
						updateClientData(resource, info.description, info.name, info.version, info.os, info.icon);
					}
					return;
				}
			}
		}

		setClientInfo(resource, "", "unknown-client");
		jreen::IQ iq(jreen::IQ::Get,presence.from());
		iq.addExtension(new jreen::Disco::Info(node));
		m_account->client()->send(iq,this,SLOT(handleIQ(jreen::IQ,int)),RequestDisco);
	}
}

void JSoftwareDetection::handleIQ(const jreen::IQ &iq, int context)
{	
	if (context == RequestSoftware) {
		if (const jreen::SoftwareVersion *soft = iq.findExtension<jreen::SoftwareVersion>().data()) {
			iq.accept();
			ChatUnit *unit = m_account->getUnit(iq.from().full(), false);
			if (JContactResource *resource = qobject_cast<JContactResource*>(unit)) {
				QString node = resource->property("node").toString();
				QString software = soft->name();
				QString softwareVersion = soft->version();
				QString os = soft->os();
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
	} else if(context == RequestDisco) {
		jreen::Disco::Info *discoInfo = iq.findExtension<jreen::Disco::Info>().data();
		if(!discoInfo)
			return;
		iq.accept();
		QString node = discoInfo->node();

		SoftwareInfo info;
		foreach(const QString &str, discoInfo->features())
			info.features << str;

		QString jid = iq.from().full();
		jreen::DataForm::Ptr form = discoInfo->form();

		if (form && form->typeName() == QLatin1String("urn:xmpp:dataforms:softwareinfo")) {
			QString software = form->field(QLatin1String("software")).value();
			QString softwareVersion = form->field(QLatin1String("software_version")).value();
			QString os = form->field(QLatin1String("os")).value();
			QString osVersion = form->field(QLatin1String("os_version")).value();
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

		m_hash.insert(node, info);

		if (JContactResource *unit = qobject_cast<JContactResource*>(m_account->getUnit(jid, false))) {
			if (unit->property("node").isNull())
				unit->setProperty("node", node);

			if (info.name.isEmpty()) {
				jreen::IQ get(jreen::IQ::Get,unit->id());
				get.addExtension(new jreen::SoftwareVersion());
				m_account->client()->send(get,this,SLOT(handleIQ(jreen::IQ,int)),RequestSoftware);
			} else {
				updateClientData(unit, info.description, info.name, info.version, info.os, info.icon);
			}
			unit->setFeatures(info.features);
		}
	}
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
	Q_UNUSED(os);
	QString desc = software;
	if (!softwareVersion.isEmpty())
		desc += " " + softwareVersion;
	return desc;
}

template <typename Char, int N>
bool isStrEqual(const QString &s1, const Char (&s2)[N])
{
	if (N != s1.size())
		return false;
	for (int i = 0; i < N; i++)
		if (s1[i] != s2[i])
			return false;
	return true;
}

QString JSoftwareDetection::getClientIcon(const QString &software)
{
	if (software.isEmpty())
		return QString();
	if (software == QLatin1String("Miranda IM Jabber"))
		return QLatin1String("miranda-jabber");
	else if (software == QLatin1String("bombusmod"))
		return QLatin1String("bombus-mod");
	else if (software == QLatin1String("bombusqd"))
		return QLatin1String("bombus-qd");
	else if (software == QLatin1String("bombus.pl"))
		return QLatin1String("bombus-pl");
	else if (software == QLatin1String("bombus+"))
		return QLatin1String("bombus-p");
	else if (isStrEqual(software, L"я.онлайн"))
		return QLatin1String("yachat");
	else if (software == QLatin1String("hotcoffee"))
		return QLatin1String("miranda-hotcoffee");
	else if (software == QLatin1String("jabber.el"))
		return QLatin1String("emacs");
	else if (software == QLatin1String("just another jabber client"))
		return QLatin1String("jajc");
	else if (isStrEqual(software, L"пиджин")) // Stupid pidgin devels! Name mustn't be localized!
		return QLatin1String("pidgin");
	return (software.toLower().replace(' ', '-') += QLatin1String("-jabber"));
}

}

/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
** Copyright © 2011 Aleksey Sidorov <gorthauer87@yandex.ru>
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
#include <jreen/iqreply.h>
#include <qutim/debug.h>
#include <qutim/json.h>
#include <jreen/error.h>
#include <QUrl>

using namespace qutim_sdk_0_3;
using namespace gloox;

namespace Jabber
{
QString toConfigNode(QString node)
{
	return node.replace(QLatin1Char('/'), QLatin1String("%2F"));
}
QString fromConfigNode(QString node)
{
	return node.replace(QLatin1String("%2F"), QChar(QLatin1Char('/')));
}
	
JSoftwareDetection::JSoftwareDetection(JAccount *account) : QObject(account)
{
	m_account = account;
	Jreen::Client *client = account->client();
	connect(client,SIGNAL(presenceReceived(Jreen::Presence)),SLOT(handlePresence(Jreen::Presence)));
	
	
	Config cache(QLatin1String("jabberhash"));
	cache.beginGroup(QLatin1String("softwareInfo"));
	foreach (const QString &node, cache.childGroups()) {
		cache.beginGroup(node);
		SoftwareInfo info;
		info.features = QSet<QString>::fromList(cache.value(QLatin1String("features"), QStringList()));
		info.name = cache.value(QLatin1String("name"), QString());
		info.version = cache.value(QLatin1String("version"), QString());
		info.os = cache.value(QLatin1String("os"), QString());
		info.icon = getClientIcon(info.name);
		info.description = getClientDescription(info.name, info.version, info.os);
		info.finished = cache.value(QLatin1String("finished"), !info.os.isEmpty());
		m_hash.insert(fromConfigNode(node), info);
		cache.endGroup();
	}
}

JSoftwareDetection::~JSoftwareDetection()
{
}

// TODO: Move to some singletone
void JSoftwareDetection::timerEvent(QTimerEvent *ev)
{
	if (ev->timerId() == m_timer.timerId()) {
		m_timer.stop();
		Config cache(QLatin1String("jabberhash"));
		cache.beginGroup(QLatin1String("softwareInfo"));
		for (int i = 0; i < m_recent.size(); i++) {
			const SoftwareInfo info = m_hash.value(m_recent[i]);
			cache.beginGroup(toConfigNode(m_recent[i]));
			cache.setValue(QLatin1String("features"), QStringList(info.features.toList()));
			cache.setValue(QLatin1String("name"), info.name);
			cache.setValue(QLatin1String("version"), info.version);
			cache.setValue(QLatin1String("os"), info.os);
			cache.setValue(QLatin1String("finished"), info.finished);
			cache.endGroup();
		}
		m_recent.clear();
	} else {
		QObject::timerEvent(ev);
	}
}

void JSoftwareDetection::handlePresence(const Jreen::Presence &presence)
{
	QString jid = presence.from().full();
	ChatUnit *unit = m_account->getUnit(jid, false);

	if (JContactResource *resource = qobject_cast<JContactResource *>(unit)) {
		if (!resource->features().isEmpty())
			return;

		QString node;
		if (Jreen::Capabilities::Ptr caps = presence.payload<Jreen::Capabilities>()) {
//			qDebug() << "handle caps" << caps->node();
			QString capsNode = caps->node();
			if(capsNode == QLatin1String("http://www.android.com/gtalk/client/caps")) {
				QString software = "GTalk (Android)";
				QString softwareVersion = caps->ver();
				QString client = getClientDescription(software, softwareVersion, QString());
				updateClientData(resource, client, software, softwareVersion, QString(), "gtalk-android");
				return;
			}
			static const QRegExp regExp("^http://.*google.com/.*client/caps$");
			Q_ASSERT(regExp.isValid());
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
				unit->setProperty("node", node);
				SoftwareInfoHash::iterator it = m_hash.find(node);
//				qDebug() << "find from hash" << m_hash.count();
				if (it != m_hash.end()) {
					SoftwareInfo &info = *it;
					resource->setFeatures(info.features);
//					qDebug() << info.name;
					if (!info.finished) {
//						qDebug() << "Send software version request";
						requestSoftware(presence.from());
					} else {
						updateClientData(resource, info.description, info.name, info.version, info.os, info.icon);
					}
					return;
				}
			}
		}

		setClientInfo(resource, "", "unknown-client");
		Jreen::Disco::Item discoItem(presence.from(), node, QString());
		Jreen::DiscoReply *reply = m_account->client()->disco()->requestInfo(discoItem);
		connect(reply, SIGNAL(finished()), SLOT(onInfoRequestFinished()));
	}
}

void JSoftwareDetection::requestSoftware(const Jreen::JID &jid)
{
	Jreen::IQ iq(Jreen::IQ::Get, jid);
	iq.addExtension(new Jreen::SoftwareVersion);
	Jreen::IQReply *reply = m_account->client()->send(iq);
	connect(reply, SIGNAL(received(Jreen::IQ)), SLOT(onSoftwareRequestFinished(Jreen::IQ)));
}

void JSoftwareDetection::onSoftwareRequestFinished(const Jreen::IQ &iq)
{
	if (Jreen::Error::Ptr error = iq.error()) {
		if (error->condition() != Jreen::Error::ServiceUnavailable)
			return;
		ChatUnit *unit = m_account->getUnit(iq.from().full(), false);
		if (JContactResource *resource = qobject_cast<JContactResource*>(unit)) {
			QString node = resource->property("node").toString();
			SoftwareInfoHash::iterator it = m_hash.find(node);
			if (it != m_hash.end()) {
				SoftwareInfo &info = (*it);
				info.finished = true;
				updateCache(node, info, true);
			}
		}
		return;
	}
	if (Jreen::SoftwareVersion::Ptr soft = iq.payload<Jreen::SoftwareVersion>()) {
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
				info.finished = true;
				info.name = software;
				info.version = softwareVersion;
//				info.os = os;
				info.icon = icon;
				info.description = client;
				updateCache(node, info, true);
			}
		}
	}
}
		
void JSoftwareDetection::onInfoRequestFinished()
{
	Jreen::DiscoReply *reply = qobject_cast<Jreen::DiscoReply*>(sender());
	Q_ASSERT(reply);
	
	if (reply->error())
		return;
	
	const Jreen::Disco::Item item = reply->item();
	const QString node = reply->item().node();
	const Jreen::DataForm::Ptr form = item.form();
	const QString jid = item.jid().full();

	SoftwareInfo info;
	info.features = item.features();

	if (form && form->typeName() == QLatin1String("urn:xmpp:dataforms:softwareinfo")) {
		QString software = form->field(QLatin1String("software")).value();
		QString softwareVersion = form->field(QLatin1String("software_version")).value();
		QString os = form->field(QLatin1String("os")).value();
		QString osVersion = form->field(QLatin1String("os_version")).value();
		QString icon = getClientIcon(software);
		QString client = getClientDescription(software, softwareVersion, os);
		debug() << Q_FUNC_INFO << software << softwareVersion << os << osVersion;
		debug() << Q_FUNC_INFO << icon << client;

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
		info.finished = !info.name.isEmpty() && !info.os.isEmpty();
	} else {
		foreach (const Jreen::Disco::Identity &identity, item.identities()) {
			if (identity.category() == QLatin1String("client")) {
				info.name = identity.name();
				info.icon = getClientIcon(info.name);
				info.description = getClientDescription(info.name, QString(), QString());
				break;
			}
		}
	}

	updateCache(node, info);

	if (JContactResource *unit = qobject_cast<JContactResource*>(m_account->getUnit(jid, false))) {
		if (unit->property("node").isNull())
			unit->setProperty("node", node);

		if (!info.finished) {
			requestSoftware(jid);
		} else {
			updateClientData(unit, info.description, info.name, info.version, info.os, info.icon);
		}
		unit->setFeatures(info.features);
	}
}

void JSoftwareDetection::updateCache(const QString &node, const SoftwareInfo &info, bool fixed)
{
	if (node.isEmpty())
		return;
	if (!fixed)
		m_hash.insert(node, info);
	m_recent << node;
	if (!m_timer.isActive())
		m_timer.start(5000, this);
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
	else if (isStrEqual(software, L"Я.онлайн"))
		return QLatin1String("yachat");
	else if (software == QLatin1String("hotcoffee"))
		return QLatin1String("miranda-hotcoffee");
	else if (software == QLatin1String("jabber.el"))
		return QLatin1String("emacs");
	else if (software == QLatin1String("just another jabber client"))
		return QLatin1String("jajc");
	else if (isStrEqual(software, L"Пиджин")) // Stupid pidgin devels! Name mustn't be localized!
		return QLatin1String("pidgin");
	return (software.toLower().replace(' ', '-') += QLatin1String("-jabber"));
}

}


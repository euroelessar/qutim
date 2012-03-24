/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#include "jvcardmanager.h"
#include "jinforequest.h"
#include "../../jprotocol.h"
#include <QDir>
#include <qutim/debug.h>
#include <qutim/rosterstorage.h>
#include <qutim/config.h>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/systeminfo.h>
#include <qutim/chatunit.h>
#include <qutim/conference.h>
#include <jreen/vcard.h>
#include <jreen/vcardupdate.h>
#include <jreen/iq.h>
#include <jreen/client.h>
#include <QCryptographicHash>
#include <QMetaProperty>

namespace Jabber
{

static bool isStatusOnline(const Status &status)
{
	Status::Type type = status.type();
	return type != Status::Offline && type != Status::Connecting;
}

JVCardManager::JVCardManager() : m_autoLoad(true)
{
}

JVCardManager::~JVCardManager()
{
}

void JVCardManager::onConnected()
{
	m_manager->fetch(m_client->jid().bareJID());
}

#define AVATAR_PATH SystemInfo::getPath(SystemInfo::ConfigDir) + QLatin1String("/avatars/jabber")

QString JVCardManager::ensurePhoto(const Jreen::VCard::Photo &photo, QString *photoPath)
{
	QString avatarHash;
	QString tmp;
	if (!photoPath)
		photoPath = &tmp;
	photoPath->clear();
	if (!photo.data().isEmpty()) {
		avatarHash = QCryptographicHash::hash(photo.data(), QCryptographicHash::Sha1).toHex();
		QDir dir(AVATAR_PATH);
		if (!dir.exists())
			dir.mkpath(dir.absolutePath());
		*photoPath = dir.absoluteFilePath(avatarHash);
		QFile file(*photoPath);
		if (file.open(QIODevice::WriteOnly)) {
			file.write(photo.data());
			file.close();
		}
	}
	return avatarHash;
}

void JVCardManager::onVCardReceived(const Jreen::VCard::Ptr &vcard, const Jreen::JID &jid)
{
	const QString avatarHash = ensurePhoto(vcard->photo());
	QList<QObject*> objects;
	if (QObject *unit = m_account->unit(jid.full(), false))
		objects << unit;
	if (jid.bare() == m_account->id())
		objects << m_account;
	foreach (QObject *object, objects) {
		if (object == m_account) {
			QString nick = vcard->nickname();
			if(nick.isEmpty())
				nick = vcard->formattedName();
			if(nick.isEmpty())
				nick = m_account->id();
			if (m_account->name() != nick)
				QMetaObject::invokeMethod(m_account, "_q_set_nick", Q_ARG(QString, nick));
			Jreen::Presence presence = m_client->presence();
			Jreen::VCardUpdate::Ptr update = presence.payload<Jreen::VCardUpdate>();
			if (update->photoHash() != avatarHash) {
				m_account->config("general").setValue("photoHash", avatarHash);
				update->setPhotoHash(avatarHash);
				m_client->send(presence);
			}
		}
		const QMetaObject * const meta = object->metaObject();
		const int index = meta->indexOfProperty("photoHash");
		if (index == -1)
			continue;
		QMetaProperty property = meta->property(index);
		const QString photoHash = property.read(object).toString();
		if (photoHash == avatarHash)
			continue;
		property.write(object, avatarHash);
	}
}

void JVCardManager::onVCardUpdateDetected(const Jreen::JID &jid, const Jreen::VCardUpdate::Ptr &update)
{
	if (!update->hasPhotoInfo())
		return;

	if (ChatUnit *unit = m_account->unit(jid.bare(), false)) {
		if (qobject_cast<Conference*>(unit))
			unit = m_account->unit(jid.full(), false);
		if (!unit)
			return;
		
		const QMetaObject * const meta = unit->metaObject();
		const int index = meta->indexOfProperty("photoHash");
		if (index == -1)
			return;
		QMetaProperty property = meta->property(index);
		if (property.read(unit).toString() == update->photoHash())
			return;
		QDir dir(AVATAR_PATH);
		if (!update->photoHash().isEmpty() && dir.exists(update->photoHash()))
			property.write(unit, update->photoHash());
		else if (m_autoLoad)
			m_manager->fetch(unit->id());
	}
}

class JAccountVCardHook : public Account
{
public:
	using Account::setInfoRequestFactory;
};

void JVCardManager::init(qutim_sdk_0_3::Account *account)
{
	m_account = account;
	Config config = m_account->protocol()->config("general");
	m_autoLoad = config.value("getavatars", true);
	m_client = qobject_cast<Jreen::Client*>(account->property("client"));
	m_manager = new Jreen::VCardManager(m_client);
	connect(m_account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)));
	connect(m_manager, SIGNAL(vCardFetched(Jreen::VCard::Ptr,Jreen::JID)),
	        SLOT(onVCardReceived(Jreen::VCard::Ptr,Jreen::JID)));
	connect(m_manager, SIGNAL(vCardUpdateDetected(Jreen::JID,Jreen::VCardUpdate::Ptr)),
	        SLOT(onVCardUpdateDetected(Jreen::JID,Jreen::VCardUpdate::Ptr)));
	connect(m_client, SIGNAL(connected()), SLOT(onConnected()));
	static_cast<JAccountVCardHook*>(account)->setInfoRequestFactory(this);
}

JVCardManager::SupportLevel JVCardManager::supportLevel(QObject *object)
{
	if (m_account == object) {
		return isStatusOnline(m_account->status()) ? ReadWrite : Unavailable;
	} else if (ChatUnit *unit = qobject_cast<ChatUnit*>(object)) {
		Account *acc = unit->account();
		if (acc == m_account)
			return isStatusOnline(acc->status()) ? ReadOnly : Unavailable;
	}
	return NotSupported;
}

InfoRequest *JVCardManager::createrDataFormRequest(QObject *object)
{
	if (m_account == object) {
		return new JInfoRequest(m_manager, m_account);
	} else if (ChatUnit *unit = qobject_cast<ChatUnit*>(object)) {
		if (unit->account() == m_account)
			return new JInfoRequest(m_manager, unit);
	}
	return 0;
}

bool JVCardManager::startObserve(QObject *object)
{
	if (m_account == object) {
		return true;
	} else if (ChatUnit *unit = qobject_cast<ChatUnit*>(object)) {
		if (unit->account() == m_account) {
			m_observedUnits.insert(unit);
			return true;
		}
	}
	return false;
}

bool JVCardManager::stopObserve(QObject *object)
{
	if (m_account == object)
		return true;
	else if (ChatUnit *contact = qobject_cast<ChatUnit*>(object))
		return m_observedUnits.remove(contact) != 0;
	return false;
}

void JVCardManager::onAccountStatusChanged(const qutim_sdk_0_3::Status &status,
										   const qutim_sdk_0_3::Status &previous)
{
	bool isOnline = isStatusOnline(status);
	bool wasOnline = isStatusOnline(previous);
	bool supported;

	if (isOnline && !wasOnline)
		supported = true;
	else if (!isOnline && wasOnline)
		supported = false;
	else
		return;

	SupportLevel level = supported ? ReadWrite : Unavailable;
	setSupportLevel(m_account, level);

	level = supported ? ReadOnly : Unavailable;
	foreach (ChatUnit *unit, m_observedUnits)
		setSupportLevel(unit, level);

}



} //namespace jabber



#include "jvcardmanager.h"
#include "jinforequest.h"
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

JVCardManager::JVCardManager() : m_autoLoad(true)
{
}

JVCardManager::~JVCardManager()
{
}

JInfoRequest *JVCardManager::fetchVCard(const QString &contact, bool create)
{
	Jreen::VCardReply *reply = m_manager->fetch(contact);
	return create ? new JInfoRequest(reply) : NULL;
}

void JVCardManager::storeVCard(const Jreen::VCard::Ptr &vcard)
{
	m_manager->store(vcard);
}

void JVCardManager::onConnected()
{
	m_manager->fetch(m_client->jid().bareJID());
}

#define AVATAR_PATH SystemInfo::getPath(SystemInfo::ConfigDir) + QLatin1String("/avatars/jabber")

void JVCardManager::onVCardReceived(const Jreen::VCard::Ptr &vcard, const Jreen::JID &jid)
{
	const Jreen::VCard::Photo &photo = vcard->photo();
	QString avatarHash;
	if (!photo.data().isEmpty()) {
		avatarHash = QCryptographicHash::hash(photo.data(), QCryptographicHash::Sha1).toHex();
		QDir dir(AVATAR_PATH);
		if (!dir.exists())
			dir.mkpath(dir.absolutePath());
		QFile file(dir.absoluteFilePath(avatarHash));
		if (file.open(QIODevice::WriteOnly)) {
			file.write(photo.data());
			file.close();
		}
	}
	QList<QObject*> objects;
	if (QObject *unit = m_account->unit(jid.full(), false))
		objects << unit;
	if (jid.node() == m_account->id())
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
		property.write(object, photoHash);
	}
}

void JVCardManager::init(qutim_sdk_0_3::Account *account)
{
	m_account = account;
	Config config = m_account->protocol()->config("general");
	m_autoLoad = config.value("getavatars", true);
	m_client = qobject_cast<Jreen::Client*>(account->property("client"));
	m_manager = new Jreen::VCardManager(m_client);
	connect(m_manager, SIGNAL(vCardFetched(Jreen::VCard::Ptr,Jreen::JID)),
	        SLOT(onVCardReceived(Jreen::VCard::Ptr,Jreen::JID)));
	connect(m_client, SIGNAL(connected()), SLOT(onConnected()));
	account->installEventFilter(this);
}

bool JVCardManager::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == InfoRequestCheckSupportEvent::eventType()) {
		Status::Type status = m_account->status().type();
		if (status >= Status::Online && status <= Status::Invisible) {
			InfoRequestCheckSupportEvent *infoEvent = static_cast<InfoRequestCheckSupportEvent*>(event);
			if (obj == m_account)
				infoEvent->setSupportType(InfoRequestCheckSupportEvent::ReadWrite);
			else
				infoEvent->setSupportType(InfoRequestCheckSupportEvent::Read);
			infoEvent->accept();
		} else {
			event->ignore();
		}
		return true;
	} else if (obj == m_account) {
		if (event->type() == QEvent::ChildAdded) {
			QObject *child = static_cast<QChildEvent*>(event)->child();
			if (ChatUnit *unit = qobject_cast<ChatUnit*>(child)) {
				if (qobject_cast<Conference*>(unit))
					return false;
				unit->installEventFilter(this);
			}
		} else if (event->type() == InfoRequestEvent::eventType()) {
			InfoRequestEvent *infoEvent = static_cast<InfoRequestEvent*>(event);
			infoEvent->setRequest(fetchVCard(m_account->id(), true));
			infoEvent->accept();
			return true;
		} else if (event->type() == InfoItemUpdatedEvent::eventType()) {
			InfoItemUpdatedEvent *infoEvent = static_cast<InfoItemUpdatedEvent*>(event);
			Jreen::VCard::Ptr vcard = JInfoRequest::convert(infoEvent->infoItem());
			storeVCard(vcard);
			event->accept();
			return true;
		}
	} else if (event->type() == InfoRequestEvent::eventType()) {
		InfoRequestEvent *infoEvent = static_cast<InfoRequestEvent*>(event);
		infoEvent->setRequest(fetchVCard(obj->property("id").toString(), true));
		infoEvent->accept();
		return true;
	}
	return QObject::eventFilter(obj, event);
}

} //namespace jabber



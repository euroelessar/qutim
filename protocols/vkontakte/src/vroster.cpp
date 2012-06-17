/****************************************************************************
**
** qutIM - instant messenger
**
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

#include "vroster.h"
#include "vrequest.h"
#include "vaccount.h"
#include "vconnection.h"
#include "vcontact.h"
#include "vroster_p.h"
#include "vaccount_p.h"
#include <QNetworkReply>
#include <qutim/debug.h>
#include <QtScript/QScriptEngine>
#include <QCryptographicHash>
#include <QFile>
#include <QDir>
#include <qutim/json.h>

#include <roster.h>

#define VK_PHOTO_ID "photo_medium"

inline void setObjectAvatar(QObject *obj, const QString &path)
{
	if (VContact *contact = qobject_cast<VContact*>(obj))
		contact->setAvatar(path);
	else if (VAccount *account = qobject_cast<VAccount*>(obj))
		account->setProperty("avatar", path);
}

void VRosterPrivate::checkPhoto(QObject *obj, const QString &photoUrl)
{
	if (photoUrl.contains(QLatin1String("question"))) {
		setObjectAvatar(obj, QString());
		return;
	}
	QString currentAvatar = obj->property("avatarUrl").toString();
	QString newAvatar = photoUrl;
	if (currentAvatar != newAvatar || obj->property("avatar").toString().isEmpty()) {
		obj->setProperty("avatarUrl", newAvatar);
		if (fetchAvatars && !avatarsQueue.contains(obj)) {
			if (avatarsQueue.isEmpty())
				QTimer::singleShot(0, this, SLOT(updateAvatar()));
			avatarsQueue.append(obj);
		}
	}
}

void VRosterPrivate::onGetProfileRequestFinished(const QVariant &var, bool error)
{
	Q_UNUSED(error);
	QVariantMap data = var.toList().value(0).toMap();
	QString name = data.value("first_name").toString() + " " + data.value("last_name").toString();
	connection->account()->setAccountName(name);
	checkPhoto(connection->account(), data.value(VK_PHOTO_ID).toString());
}

void VRosterPrivate::onGetTagListRequestFinished(const QVariant &var, bool error)
{
	Q_Q(VRoster);
	Q_UNUSED(error);
	foreach (QVariant data, var.toList()) {
		QVariantMap map = data.toMap();
		tags.insert(map.value("lid").toInt(), map.value("name").toString());
	}
	q->getFriendList();
}

void VRosterPrivate::onGetFriendsRequestFinished(const QVariant &var, bool error)
{
	Q_UNUSED(error);
	QVariantList friends = var.toList();

	//let's try again
	if (friends.isEmpty())
		QTimer::singleShot(5000 ,q_func(), SLOT(getFriendList()));

	foreach (const QVariant &var, friends) {
		QVariantMap data = var.toMap();
		debug() << data;
		QString id = data.value("uid").toString();
		VContact *contact = contacts.value(id);
		bool shouldInit = false;
		if (!contact) {
			contact = connection->account()->getContact(id, true);
			shouldInit = true;
		} else if (!contact->isInList()) {
			shouldInit = true;
		}
		if (shouldInit) {
			QString firstName = data.value("first_name").toString();
			QString lastName = data.value("last_name").toString();
			contact->setContactName(firstName + " " + lastName);
			QStringList contactTags;
			foreach (const QString &tagId, data.value("lists").toStringList())
				contactTags << tags.value(tagId.toInt());
			contact->setContactTags(contactTags);
			contact->setContactInList(true);
			storage->addContact(contact);
		} /*else
			storage->updateContact(c);*/
		checkPhoto(contact, data.value(VK_PHOTO_ID).toString());
		contact->setOnline(data.value("online").toInt() == 1);
		contact->setProperty("mobilePhone",data.value("mobile_phone"));
	}
}

void VRosterPrivate::onActivityUpdateRequestFinished(const QVariant &var, bool error)
{
	Q_UNUSED(error);
	QVariantList activities = var.toList();
	for (int i = 1; i < activities.size(); i++) {
		QVariantMap data = activities.at(i).toMap();
		QString id = data.value("uid").toString();
		QString text = data.value("text").toString();
		VContact *contact = connection->account()->getContact(id, false);
		if (contact)
			contact->setActivity(text);
	}

	lastActivityTime = QDateTime::currentDateTime();
}

QString VRosterPrivate::photoHash(const QString &path)
{
	QCryptographicHash hash(QCryptographicHash::Md5);
	hash.addData(path.toUtf8());
	return hash.result().toHex();
}

void VRosterPrivate::onAvatarRequestFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	reply->deleteLater();
	QObject *obj = reinterpret_cast<QObject*>(reply->property("object").value<qptrdiff>());
	QString hash = reply->property("hash").toString();
	Q_ASSERT(obj);
	QDir dir = getAvatarsDir();
	if ( !dir.exists() )
		dir.mkdir(dir.path());

	//FIXME
	QFile file(dir.filePath(hash));
	if (!file.exists()) {
		if (!file.open(QIODevice::WriteOnly))
			return;
		file.write(reply->readAll());
		file.close();
	}
	setObjectAvatar(obj, file.fileName());
	QTimer::singleShot(0, this, SLOT(updateAvatar()));
}

void VRosterPrivate::updateAvatar()
{
	Q_Q(VRoster);
	if (avatarsQueue.isEmpty()) {
		return;
	}

	QObject *obj = avatarsQueue.dequeue();
	q->requestAvatar(obj);
}

void VRosterPrivate::updateActivity()
{
	QVariantMap data;
	if (lastActivityTime.isValid()) {
		data.insert("timestamp", lastActivityTime.toUTC().toTime_t());
	} else {
		data.insert("timestamp", 0);
		// TODO smth clever
	}

	VReply *reply = connection->request("activity.getNews", data);
	connect(reply, SIGNAL(resultReady(QVariant,bool)), this, SLOT(onActivityUpdateRequestFinished(QVariant,bool)));
}

void VRosterPrivate::onSetActivityRequestFinished(const QVariant &var, bool error)
{
	Q_UNUSED(var);
	Q_UNUSED(error);
	// May be we should do something?
}

void VRosterPrivate::onConnectStateChanged(VConnectionState state)
{
	Q_Q(VRoster);
	switch (state) {
	case Connected: {
		q->getTagList();
		q->getProfile();
		friendListUpdater.start();
		if (getActivity)
			activityUpdater.start();
		break;
	}
	case Disconnected: {
		friendListUpdater.stop();
		activityUpdater.stop();
		foreach (VContact *c, connection->account()->contacts())
			c->setOnline(false);
		break;
	}
	default: {
		break;
	}
	}
}

VRoster::VRoster(VConnection* connection) : QObject(connection),
	d_ptr(new VRosterPrivate)
{
	Q_D(VRoster);
	d->connection = connection;
	d->q_ptr = this;
	d->storage = RosterStorage::instance();
	d->connection->account()->setContactsFactory(d);
	loadSettings();
	connect(connection,SIGNAL(connectionStateChanged(VConnectionState)),d,SLOT(onConnectStateChanged(VConnectionState)));
	connect(&d->friendListUpdater,SIGNAL(timeout()),this,SLOT(getFriendList()));
	connect(&d->avatarsUpdater,SIGNAL(timeout()),d,SLOT(updateAvatar()));
	connect(&d->activityUpdater,SIGNAL(timeout()),d,SLOT(updateActivity()));
}

VRoster::~VRoster()
{

}

VContact* VRoster::getContact(const QString &uid, bool create)
{
	Q_D(VRoster);
	VContact *contact = d->contacts.value(uid);
	if (create && !contact) {
		contact = new VContact(uid, d->connection->account());
		d->contacts.insert(uid, contact);
		emit d->connection->account()->contactCreated(contact);
		connect(contact,SIGNAL(destroyed(QObject*)),d,SLOT(onContactDestroyed(QObject*)));
	}
	return contact;
}

VContact* VRoster::getContact(const QVariantMap &data, bool create)
{
	Q_D(VRoster);
	QString id = data.value("uid").toString();
	VContact *c = d->contacts.value(id, 0);
	if (!c) {
		if (create)
			c = getContact(id, true);
		else
			return 0;
	}

	QString firstName = data.value("first_name").toString();
	QString lastName = data.value("last_name").toString();
	c->setContactName(firstName + " " + lastName);
	d->checkPhoto(c, data.value(VK_PHOTO_ID).toString());
	return c;
}

void VRoster::getProfile()
{
	Q_D(VRoster);
	QVariantMap data;
	data.insert("uids", d->connection->account()->uid());
	data.insert("fields", "first_name,last_name,nickname," VK_PHOTO_ID);
	VReply *reply = d->connection->request("getProfiles", data);
	connect(reply, SIGNAL(resultReady(QVariant,bool)), d, SLOT(onGetProfileRequestFinished(QVariant,bool)));
}

void VRoster::getTagList()
{
	Q_D(VRoster);
	VReply *reply = d->connection->request("friends.getLists");
	connect(reply, SIGNAL(resultReady(QVariant,bool)), d, SLOT(onGetTagListRequestFinished(QVariant,bool)));
}

void VRoster::getFriendList()
{
	Q_D(VRoster);
	QVariantMap data;
	data.insert("fields", "uid,first_name,last_name,nickname,bdate," VK_PHOTO_ID ",online,lists,contacts,mobile_phone");
	VReply *reply = d->connection->request("friends.get", data);
	connect(reply, SIGNAL(resultReady(QVariant,bool)), d, SLOT(onGetFriendsRequestFinished(QVariant,bool)));
}

void VRoster::requestAvatar(QObject *obj)
{
	Q_D(VRoster);
	QString path = obj->property("avatarUrl").toString();
	if (path.contains(QLatin1String("question"))) {
		setObjectAvatar(obj, QString());
		QTimer::singleShot(0, d, SLOT(updateAvatar()));
		return;
	}
	QDir dir = getAvatarsDir();
	QString hash = d->photoHash(path);
	if (dir.exists(hash)) {
		setObjectAvatar(obj, dir.filePath(hash));
		QTimer::singleShot(0, d, SLOT(updateAvatar()));
		return;
	}
	QNetworkAccessManager *networkManager = static_cast<QNetworkAccessManager*>(d->connection);
	QNetworkReply *reply = networkManager->get(QNetworkRequest(path));
	reply->setProperty("hash", hash);
	reply->setProperty("object", reinterpret_cast<qptrdiff>(obj));
	connect(reply, SIGNAL(finished()), d, SLOT(onAvatarRequestFinished()));
}

void VRoster::setActivity(const Status &activity)
{
	Q_D(VRoster);
	if (activity.text().isEmpty())
		return;
	QVariantMap data;
	data.insert("text", activity.text());
	VReply *reply = d->connection->request("activity.set", data);
	connect(reply, SIGNAL(resultReady(QVariant,bool)), d, SLOT(onSetActivityRequestFinished(QVariant,bool)));
}

ConfigGroup VRoster::config()
{
	return d_func()->connection->config("roster");
}

void VRoster::loadSettings()
{
	Q_D(VRoster);
	int interval;
	Config cfg = config();
	d->storage->load(d->connection->account());
	d->friendListUpdater.setInterval(cfg.value("friendListUpdateInterval", 600000));
	cfg.beginGroup("avatars");
	interval = cfg.value("interval", 5000);
	d->fetchAvatars = (interval > 0);
	d->avatarsUpdater.setInterval(interval);
	cfg.endGroup();
	cfg.beginGroup("activity");
	interval = cfg.value("interval", 5000);
	d->getActivity = (interval > 0);
	d->activityUpdater.setInterval(interval);
}

void VRoster::saveSettings()
{

}

void VRosterPrivate::serialize(Contact *generalContact, QVariantMap &data)
{
	VContact *contact = qobject_cast<VContact*>(generalContact);
	if (!contact)
		return;
	data.insert(QLatin1String("id"),contact->id());
	data.insert(QLatin1String("name"), contact->name());
	data.insert(QLatin1String("tags"), contact->tags());
	data.insert(QLatin1String("inList"),contact->id());
	data.insert(QLatin1String("avatarPath"), contact->avatar());
	data.insert(QLatin1String("avatarUrl"), contact->property("avatarUrl"));
	data.insert(QLatin1String("activity"), contact->activity());
}

Contact *VRosterPrivate::addContact(const QString &id, const QVariantMap &data)
{
	VContact *c = q_func()->getContact(id,true);
	c->setContactName(data.value(QLatin1String("name")).toString());
	c->setContactTags(data.value(QLatin1String("tags")).toStringList());
	c->setContactInList(data.value(QLatin1String("inList")).toBool());
	c->setAvatar(data.value(QLatin1String("avatarPath")).toString());
	c->setProperty("avatarUrl",data.value(QLatin1String("avatarUrl")));
	c->setActivity(data.value(QLatin1String("activity")).toString());

	return c;
}

void VRoster::updateProfile(VContact *contact)
{
	Q_D(VRoster);
	QString id = contact->id();
	QVariantMap data;
	data.insert("uids",id) ;
	data.insert("fields", "first_name,last_name,nickname," VK_PHOTO_ID);
	VReply *reply = d->connection->request("getProfiles", data);
	reply->setProperty("contact",qVariantFromValue(contact));
	connect(reply, SIGNAL(resultReady(QVariant,bool)), d, SLOT(onUpdateProfileFinished(QVariant,bool)));
}

void VRosterPrivate::onUpdateProfileFinished(const QVariant &var, bool error)
{
	Q_UNUSED(error);
	VContact *contact = sender()->property("contact").value<VContact*>();
	QVariantMap data = var.toList().value(0).toMap();
	QString name = data.value("first_name").toString() + " " + data.value("last_name").toString();
	contact->setContactName(name);
	checkPhoto(contact, data.value(VK_PHOTO_ID).toString());
}

void VRosterPrivate::onContactDestroyed(QObject *obj)
{
	VContact *contact = reinterpret_cast<VContact*>(obj);
	contacts.remove(contacts.key(contact));
}

namespace playground {

VRoster::VRoster(vk::Roster *roster) : QObject(roster),
	m_roster(roster)
{
}

VContact *VRoster::contact(int id)
{
}

VContact *VRoster::contact(int id) const
{
}

} //namespace playground

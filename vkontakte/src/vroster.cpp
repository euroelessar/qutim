/****************************************************************************
 *  vroster.cpp
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
 *                     by Ruslan Nigmatullin <euroelessar@gmail.com>
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

void VRosterPrivate::checkPhoto(QObject *obj, const QString &photoUrl)
{
	QString currentAvatar = obj->property("avatarUrl").toString();
	QString newAvatar = photoUrl;
	if (currentAvatar != newAvatar) {
		obj->setProperty("avatarUrl", newAvatar);
		if (fetchAvatars && !avatarsQueue.contains(obj)) {
			if (avatarsQueue.isEmpty())
				QTimer::singleShot(0, this, SLOT(updateAvatar()));
			avatarsQueue.append(obj);
		}
	}
}

void VRosterPrivate::onGetProfileRequestFinished()
{
	Q_Q(VRoster);
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	QByteArray rawData = reply->readAll();
	qDebug() << Q_FUNC_INFO << rawData;
	QVariantMap data = Json::parse(rawData).toMap().value("response").toList().value(0).toMap();
	QString name = data.value("first_name").toString() + " " + data.value("last_name").toString();
	connection->account()->setAccountName(name);
	checkPhoto(connection->account(), data.value("photo_medium").toString());
}

void VRosterPrivate::onGetTagListRequestFinished()
{
	Q_Q(VRoster);
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	QByteArray rawData = reply->readAll();
	qDebug() << Q_FUNC_INFO << rawData;
	QVariantMap tagData = Json::parse(rawData).toMap().value("response").toMap();
	QVariantMap::const_iterator it = tagData.constBegin();
	for (; it != tagData.constEnd(); it++)
		tags.insert(it.key(), it.value().toString());
	q->getFriendList();
}

void VRosterPrivate::onGetFriendsRequestFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	//	qDebug() << reply->url();
	QByteArray rawData = reply->readAll();
	//	qDebug() << rawData;
	QVariantList friends = Json::parse(rawData).toMap().value("response").toList();
	//	qDebug() << friends;
	foreach (const QVariant &var, friends) {
		QVariantMap data = var.toMap();
		QString id = data.value("uid").toString();
		VContact *c = contacts.value(id, 0);
		bool shouldInit = false;
		if (!c) {
			c = connection->account()->getContact(id, true);
			shouldInit = true;
		} else if (!c->isInList()) {
			shouldInit = true;
		}
		if (shouldInit) {
			QString firstName = data.value("first_name").toString();
			QString lastName = data.value("last_name").toString();
			c->setContactName(firstName + " " + lastName);
			//			qDebug() << data.value("lists") << tags;
			QStringList contactTags;
			foreach (const QString &tagId, data.value("lists").toStringList())
				contactTags << tags.value(tagId);
			c->setContactTags(contactTags);
			c->setContactInList(true);
		}
		checkPhoto(c, data.value("photo_rec").toString());
		c->setStatus(data.value("online").toInt() == 1);
	}

	//    QScriptEngine engine;
	//    QScriptValue sc_data = engine.evaluate('(' + data + ')');
	//    QScriptValue sc_cnt = sc_data.property("friends").property("n");
	//
	//	if (fetchAvatars)
	//		avatarsUpdater.stop();
	//
	//    for (int i=0; i<sc_cnt.toInteger(); i++) {
	//		QScriptValue sc_item = sc_data.property("friends").property("d").property(i);
	//		QString id = sc_item.property(0).toString();
	//		VContact *c = connection->account()->getContact(id,false);
	//		if (!c) {
	//			c = connection->account()->getContact(id,true);
	//			c->setName(sc_item.property(1).toString());
	//			QStringList tags;
	//			tags << tr("Friends");
	//			c->setTags(tags);
	//			c->setInList(true);
	//		}
	//		QString current_avatar = c->property("avatarUrl").toString();
	//		QString new_avatar = sc_item.property(2).toString();
	//		if (current_avatar != new_avatar) {
	//			c->setProperty("avatarUrl",new_avatar);
	//			if (fetchAvatars && !avatarsQueue.contains(c))
	//				avatarsQueue.append(c);
	//		}
	//		c->setStatus(sc_item.property(3).toBoolean());
	//    }
	//	if (!avatarsQueue.isEmpty() && !avatarsUpdater.isActive())
	//		avatarsUpdater.start();
}

void VRosterPrivate::onActivityUpdateRequestFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);

	QVariantMap map = Json::parse(reply->readAll()).toMap();
	
	QVariantList activities = map.value("response").toList();
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
	if (VContact *contact = qobject_cast<VContact*>(obj))
		contact->setAvatar(file.fileName());
	else if (VAccount *account = qobject_cast<VAccount*>(obj))
		account->setProperty("avatar", file.fileName());
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
	Q_Q(VRoster);
	QVariantMap data;
	if (lastActivityTime.isValid()) {
		data.insert("timestamp", lastActivityTime.toUTC().toTime_t());
	} else {
		// TODO smth clever
	}
	
	QNetworkReply *reply = connection->get("activity.getNews", data);
	connect(reply, SIGNAL(finished()), this, SLOT(onActivityUpdateRequestFinished()));
	
	//	QList<VContact *> contact_list = connection->account()->d_func()->contactsList;
	//	if (contact_list.isEmpty())
	//		return;
	//	int index =  activityUpdater.property("index").toInt();
	//	q->requestActivity(contact_list.at(index));
	//	index++;
	//	if (index == contact_list.count())
	//		index = 0;
	//	activityUpdater.setProperty("index",QVariant(index));
}

void VRosterPrivate::onSetActivityRequestFinished()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	Q_ASSERT(reply);
	QVariantMap map = Json::parse(reply->readAll()).toMap();

	QString response = map.value("response").toString();
	Q_UNUSED(response); //i don't know, what it makes
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
			//			avatarsUpdater.stop();
			foreach (VContact *c,connection->account()->findChildren<VContact *>()) {
				c->setStatus(false);
			}
			break;
		}
	default: {
			break;
		}		
	}
}

VRoster::VRoster(VConnection* connection, QObject* parent) : QObject(parent),d_ptr(new VRosterPrivate)
{
	Q_D(VRoster);
	d->connection = connection;
	d->q_ptr = this;
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
	d->checkPhoto(c, data.value("photo_rec").toString());
	return c;
}

void VRoster::getProfile()
{
	Q_D(VRoster);
	QVariantMap data;
	data.insert("uids", d->connection->account()->uid());
	data.insert("fields", "first_name,last_name,nickname,photo_medium");
	QNetworkReply *reply = d->connection->get("getProfiles", data);
	connect(reply,SIGNAL(finished()),d,SLOT(onGetProfileRequestFinished()));
}

void VRoster::getTagList()
{
	Q_D(VRoster);
	QNetworkReply *reply = d->connection->get("friends.getLists");
	connect(reply,SIGNAL(finished()),d,SLOT(onGetTagListRequestFinished()));
}

void VRoster::getFriendList()
{
	Q_D(VRoster);
	QVariantMap data;
	data.insert("fields", "uid,first_name,last_name,nickname,bdate,photo_rec,online,lists");
	QNetworkReply *reply = d->connection->get("friends.get", data);
	connect(reply,SIGNAL(finished()),d,SLOT(onGetFriendsRequestFinished()));
}

void VRoster::requestAvatar(QObject *obj)
{
	Q_D(VRoster);
	QString path = obj->property("avatarUrl").toString();
	QDir dir = getAvatarsDir();
	QString hash = d->photoHash(path);
	if (dir.exists(hash)) {
		d->checkPhoto(obj, dir.filePath(hash));
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
	QVariantMap data;
	data.insert("text", activity.text());
	//WTF?
	//QNetworkReply *reply = d->connection->get("activity.set", data);
	//connect(reply,SIGNAL(finished()),d,SLOT(onSetActivityRequestFinished()));
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
	int size = cfg.beginArray("list");
	for (int i = 0; i < size; i++) {
		cfg.setArrayIndex(i);
		QString id = cfg.value("id", QString());
		if (id.isEmpty())
			continue;
		VContact *contact = getContact(id, true);
		contact->setContactInList(cfg.value("inList",true));
		contact->setContactName(cfg.value("name", QString()));
		contact->setContactTags(cfg.value("tags", QStringList()));
		cfg.beginGroup("avatar");
		contact->setAvatar(cfg.value("path", QString()));
		contact->setProperty("avatarUrl", cfg.value("url", QString()));
		cfg.endGroup();
		contact->setActivity(cfg.value("activity", QString()));
	}
	cfg.endArray();
	d->friendListUpdater.setInterval(cfg.value("friendListUpdateInterval", 3600000));
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
	Q_D(VRoster);
	Config cfg = config();
	cfg.remove("list");
	cfg.beginArray("list");
	int index = 0;
	foreach (VContact *contact, d->contacts) {
		cfg.setArrayIndex(index++);
		cfg.setValue("id", contact->id());
		cfg.setValue("name", contact->name());
		cfg.setValue("tags", contact->tags());
		cfg.setValue("activity", contact->activity());
		cfg.setValue("inList", contact->isInList());
		cfg.beginGroup("avatar");
		cfg.setValue("url", contact->property("avatarUrl"));
		cfg.setValue("path", contact->avatar());
		cfg.endGroup();
	}
}

void VRoster::updateProfile(VContact *contact)
{
	Q_D(VRoster);
	QString id = contact->id();
	QVariantMap data;
	data.insert("uids",id) ;
	data.insert("fields", "first_name,last_name,nickname,photo_medium");
	QNetworkReply *reply = d->connection->get("getProfiles", data);
	reply->setProperty("contact",qVariantFromValue(contact));
	connect(reply,SIGNAL(finished()),d,SLOT(onUpdateProfileFinished()));
}

void VRosterPrivate::onUpdateProfileFinished()
{
	Q_Q(VRoster);
	QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
	VContact *contact = reply->property("contact").value<VContact*>();
	QByteArray rawData = reply->readAll();
	qDebug() << Q_FUNC_INFO << rawData;
	QVariantMap data = Json::parse(rawData).toMap().value("response").toList().value(0).toMap();
	QString name = data.value("first_name").toString() + " " + data.value("last_name").toString();
	contact->setContactName(name);
	checkPhoto(contact, data.value("photo_medium").toString());
}

void VRosterPrivate::onContactDestroyed(QObject *obj)
{
	VContact *contact = reinterpret_cast<VContact*>(obj);
	contacts.remove(contacts.key(contact));
}

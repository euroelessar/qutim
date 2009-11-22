/*
    VprotocolWrap

		Copyright (c) 2009 by Rustam Chakin <qutim.develop@gmail.com>
									2009 by Alexander Kazarin <boiler@co.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include <QSettings>
#include <QNetworkCookieJar>
#include <QMessageBox>
#include <QTextDocument>

#include "../core/vpluginsystem.h"
#include "vprotocolwrap.h"

VprotocolWrap::VprotocolWrap(const QString &account_name, const QString &profile_name) :
	m_account_name(account_name),
	m_profile_name(profile_name)
{
    m_network_handler = new QNetworkAccessManager(this);
    connect(m_network_handler, SIGNAL(finished(QNetworkReply*)), this, SLOT(getReply(QNetworkReply*)));

    m_network_request.setRawHeader("User-Agent", "qutIM plugin VKontakte");
    m_network_request.setRawHeader("Accept-Charset", "utf-8");
    m_network_request.setRawHeader("Pragma", "no-cache");
    m_network_request.setRawHeader("Cache-control", "no-cache");

    m_current_state = Offline;
    m_new_messages_count = 0;

    // create timers
    prolongation = new QTimer(this);
    connect(prolongation, SIGNAL(timeout()), this, SLOT(sendProlongation()));
    friendsUpdate = new QTimer(this);
    connect(friendsUpdate, SIGNAL(timeout()), this, SLOT(sendRequestForFriendList()));
    messagesUpdate = new QTimer(this);
    connect(messagesUpdate, SIGNAL(timeout()), this, SLOT(checkForNewMessages()));
    newsUpdate = new QTimer(this);
    connect(newsUpdate, SIGNAL(timeout()), this, SLOT(sendRequestForNews()));

    loadSettings();
}

VprotocolWrap::~VprotocolWrap()
{
    sendLogout();
    delete m_network_handler;
}

void VprotocolWrap::loadSettings()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name+"/vkontakte."+m_account_name, "accountsettings");
    m_refresh_friend_list = settings.value("main/friends", 60).toUInt();
    m_check_for_messages_every = settings.value("main/checkmess", 60).toUInt();
    m_check_for_news_every = settings.value("main/checknews", 300).toUInt();
    m_news_photo_enable = settings.value("news/photo_enable", true).toBool();
    m_news_photo_preview = settings.value("news/photo_preview", true).toBool();
    m_news_photo_fullsize = settings.value("news/photo_fullsize", false).toBool();
}

void VprotocolWrap::sendAuthorisation()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "qutim/qutim."+m_profile_name+"/vkontakte."+m_account_name, "accountsettings");

    // encode password with Percent Encoding
    QByteArray pass = settings.value("main/password","").toByteArray();
    pass = pass.toPercentEncoding("","");

    // construct our URL
    QString sQuery = QString("login=force&site=2&email=%1&pass=")
				  .arg(settings.value("main/name","").toString()) + pass;
    QUrl url("http://login.userapi.com/auth?");
    url.setEncodedQuery(sQuery.toUtf8());

    m_network_request.setUrl(url);
    //qDebug() << "VprotocolWrap::sendAuthorisation(" << url.toEncoded() << ")";
    m_network_handler->get(m_network_request);
    m_current_state = Connecting;
}

void VprotocolWrap::getReply(QNetworkReply *reply)
{
    QString sFunction;
    QString sIncomingData = QString::fromUtf8(reply->readAll());
    if ( !sIncomingData.isEmpty() )
    {
	// decompose data
	sFunction = sIncomingData.left(sIncomingData.indexOf("(",0,Qt::CaseInsensitive));
	sIncomingData = sIncomingData.mid(sFunction.size() + 1, sIncomingData.size() - sFunction.size() - 3);
    }
    else
    {
	//foreach( QByteArray header, reply->rawHeaderList())
	//{
	//    qDebug() << "header: " << header << reply->rawHeader(header);
	//}
	if (m_current_state == ObsoleteSID)
	{
	    QByteArray location = reply->rawHeader("Location");
	    int tmp_index = location.indexOf("sid=");
	    if (tmp_index > -1 )
	    {
		qDebug() << "Old sid: " << m_mine_sid;
		m_mine_sid = location.mid(tmp_index + 4);
		qDebug() << "New sid: " << m_mine_sid;
		m_current_state = Online;
	    }
	}
    }
    //qDebug() << "reply: " << sIncomingData;
    if ( m_current_state == Connecting ) {
	QList<QNetworkCookie> cookie_list = m_network_handler->cookieJar()->cookiesForUrl(QUrl("http://login.userapi.com"));
	bool sid_founded = false;
	QByteArray location = reply->rawHeader("Location");
	int tmp_index = location.indexOf("sid=");
	if (tmp_index > -1 )
	{

	    foreach(QNetworkCookie cookie, cookie_list)
	    {
		if ( cookie.name() == "remixpassword" && cookie.value() != "deleted" )
		{
		    m_remixpassword = cookie.value();
		    sid_founded = true;
		    m_mine_sid = location.mid(tmp_index + 4);
		    qDebug() << "Session ID was found: " << m_mine_sid;
		    m_current_state = Online;
		    emit iMConnected();

		    //prolongation->start(m_keep_alive_every * 1000);
		    friendsUpdate->start(m_refresh_friend_list * 1000);
		    messagesUpdate->start(m_check_for_messages_every * 1000);
		    if (m_check_for_news_every>0)
			newsUpdate->start(m_check_for_news_every * 1000);

		    //sendProlongation();
		    sendRequestForFriendList();
		    checkForNewMessages();
		    if (m_check_for_news_every>0) sendRequestForNews();
		}
		if ( cookie.name() == "remixmid" )
		    m_mine_id = cookie.value();
	    }

	}
	if (!sid_founded)
	{
	    qDebug() << "Session ID was not found. Mismatch nick or password?";
	    m_current_state = Offline;
	    VpluginSystem::instance().sendSystemMessage(m_account_name, tr("Mismatch nick or password"));
	    emit iMDisconnected();
	}
    }
    else if (m_current_state == Online && !sIncomingData.isEmpty() ) {
	int errorCode = checkForErrorReply(sIncomingData);
	if (errorCode == 1) // no error
	{
	    if ( sFunction  == "getProfile" ) {
		processProfileData(sIncomingData);
	    }
	    else if ( sFunction  == "getFaves" ) {
		processFaves(sIncomingData);
	    }
	    else if ( sFunction  == "getActivity" ) {
		processActiveData(sIncomingData);
	    }
	    else if ( sFunction  == "getNewMessages" )	{
		setNewMessagesCount(sIncomingData);
	    }
	    else if ( sFunction  == "getLastMessages" ) {
		processMessagesArray(sIncomingData);
	    }
	    else if ( sFunction  == "getNews" ) {
		processNews(sIncomingData);
	    }
	}
	else // userapi replied an error
	{
	    qDebug() << "USERAPI ERROR: " << sFunction << sIncomingData;
	    if (errorCode == -1) // authorization error
	    {
		m_current_state = ObsoleteSID;
		sendProlongation(); // trying to obtain new sid
	    }
	    if ( sFunction  == "sendMessage" ) {
		QMessageBox::critical(0, "Error", QString("Message was not delivered successfully. error_code = %1")
				      .arg(errorCode));
	    }
	}
    }
    reply->deleteLater();

}

int VprotocolWrap::checkForErrorReply(QString data)
{
    QScriptEngine sc_engine;
    QScriptValue sc_data = sc_engine.evaluate(data);
    QScriptValue error_code = sc_data.property("ok");
    if ( error_code.isValid() )
    {
	qDebug() << "VprotocolWrap::checkForErrorReply =" << error_code.toInteger();
	return error_code.toInteger();
    }
    else
    { // not an error reply";
	return 1; // 1 == ok
    }
}

void VprotocolWrap::sendProlongation()
{
    if ( m_current_state != Offline || m_current_state != Connecting )
    {
	qDebug() << "VprotocolWrap::sendProlongation()";
	m_network_request.setRawHeader("remixpassword", m_remixpassword);
	m_network_request.setUrl(QUrl("http://login.userapi.com/auth?login=auto&site=2"));
	m_network_handler->get(m_network_request);
    }
}

void VprotocolWrap::stopTimers()
{
    qDebug() << "VprotocolWrap::stopTimers()";
    prolongation->stop();
    friendsUpdate->stop();
    messagesUpdate->stop();
    newsUpdate->stop();
}

void VprotocolWrap::sendLogout()
{
    stopTimers();
    qDebug() << "VprotocolWrap::sendLogout()";

    m_network_request.setUrl(QUrl(QString("http://login.userapi.com/auth?login=logout&site=2&sid=%1")
				  .arg(QString::fromUtf8(m_mine_sid))));
    m_network_handler->get(m_network_request);
    m_current_state = Offline;
}

void VprotocolWrap::sendRequestForFriendList()
{
    if (m_current_state == Online)
    {
	//qDebug() << "VprotocolWrap::sendRequestForFriendList(" << m_mine_id << "," << m_mine_sid << ")";
	m_network_request.setUrl(QUrl(QString("http://userapi.com/data?friends=0-1000000&id=%1&sid=%2&back=getProfile")
				      .arg(QString::fromUtf8(m_mine_id))
				      .arg(QString::fromUtf8(m_mine_sid))));
	m_network_handler->get(m_network_request);

	askForFriendsActivity();

	// Bookmarks request
	m_network_request.setUrl(QUrl(QString("http://userapi.com/data?fave=0-1000000&id=%1&sid=%2&back=getFaves")
				      .arg(QString::fromUtf8(m_mine_id))
				      .arg(QString::fromUtf8(m_mine_sid))));
	m_network_handler->get(m_network_request);
    }
}

void VprotocolWrap::processProfileData(QString data)
{
    //	qDebug()<<"processProfileData"<<data;
    data.replace("\\/","/");
    data.replace("\\t", " ");

    QScriptEngine sc_engine;
    QScriptValue sc_data = sc_engine.evaluate(data);
    QScriptValue sc_cnt = sc_data.property("friends").property("n");
    QList<FriendBuddy> qutim_friends;

    // Contact for updates (news) notifications
    FriendBuddy f_buddy;
    f_buddy.m_id = "news";
    f_buddy.m_name = tr("Vkontakte.ru updates");
    f_buddy.m_online = false;
    qutim_friends.append(f_buddy);

    for (int i=0; i<sc_cnt.toInteger(); i++) {
	QScriptValue sc_item = sc_data.property("friends").property("d").property(i);
	FriendBuddy f_buddy;
	f_buddy.m_id = sc_item.property(0).toString();
	f_buddy.m_name = sc_item.property(1).toString();
	f_buddy.m_avatar_url = sc_item.property(2).toString();
	f_buddy.m_online = sc_item.property(3).toBoolean();
	qutim_friends.append(f_buddy);
	m_friend_names[f_buddy.m_id] = f_buddy.m_name;
	//qDebug()<<"processProfileData"<<f_buddy.m_id<<f_buddy.m_name<<f_buddy.m_avatar_url<<f_buddy.m_online;
    }
    if (qutim_friends.count()) emit friendListArrived(qutim_friends);
}

void VprotocolWrap::processFaves(QString data)
{
    data.replace("\\/","/");
    data.replace("\\t", " ");

    QScriptEngine sc_engine;
    QScriptValue sc_data = sc_engine.evaluate(data);
    QScriptValue sc_cnt = sc_data.property("fave").property("n");
    QList<FriendBuddy> qutim_faves;

    for (int i=0; i<sc_cnt.toInteger(); i++) {
	QScriptValue sc_item = sc_data.property("fave").property("d").property(i);
	FriendBuddy f_buddy;
	f_buddy.m_id = sc_item.property(0).toString();
	f_buddy.m_name = sc_item.property(1).toString();
	f_buddy.m_avatar_url = sc_item.property(2).toString();
	f_buddy.m_online = sc_item.property(3).toBoolean();
	qutim_faves.append(f_buddy);
	//		qDebug()<<"processFaves"<<f_buddy.m_id<<f_buddy.m_name<<f_buddy.m_avatar_url<<f_buddy.m_online;
    }

    emit faveListArrived(qutim_faves);
}

void VprotocolWrap::askForFriendsActivity()
{
    QNetworkRequest request;
    m_network_request.setUrl(QUrl(QString("http://userapi.com/data?updates_activities=0-1000000&id=%1&sid=%2&back=getActivity")
				  .arg(QString::fromUtf8(m_mine_id))
				  .arg(QString::fromUtf8(m_mine_sid))));
    m_network_handler->get(m_network_request);
}

void VprotocolWrap::processActiveData(QString data)
{
    //	qDebug()<<"processActiveData"<<data;
    data.replace("\\/","/");
    data.replace("\\t", " ");

    QScriptEngine sc_engine;
    QScriptValue sc_data = sc_engine.evaluate(data);
    QScriptValue sc_cnt = sc_data.property("updates_activities").property("n");
    QList<Activity> qutim_activities;

    for (int i=0; i<sc_cnt.toInteger(); i++) {
	QScriptValue sc_item = sc_data.property("updates_activities").property("d").property(i);
	Activity f_act;
	f_act.m_id = sc_item.property(1).toString();
	f_act.m_activity = sc_item.property(5).toString();
	qutim_activities.prepend(f_act);
	//		qDebug()<<"processActiveData"<<f_act.m_id<<f_act.m_activity;
    }
    if (qutim_activities.count()) emit activitiesListArrived(qutim_activities);
}

void VprotocolWrap::sendMessage(const QString &buddy_id, const QString &message)
{
    m_network_request.setUrl(QUrl(QString("http://userapi.com/data?act=add_message&id=%1&sid=%2&ts=%3&message=%4&back=sendMessage")
				  .arg(buddy_id)
				  .arg(QString::fromUtf8(m_mine_sid))
				  .arg(QDateTime::currentDateTime().toTime_t())
				  .arg(message)
				  ));
    //qDebug() << "VprotocolWrap::sendMessage" << m_network_request.url();
    m_network_handler->get(m_network_request);
}

void VprotocolWrap::checkForNewMessages()
{
    //qDebug() << "VprotocolWrap::checkForNewMessages()";
    if (m_current_state == Online)
    {
	m_network_request.setUrl(QUrl(QString("http://userapi.com/data?act=history&id=%1&sid=%2&back=getNewMessages")
				      .arg(0)
				      .arg(QString::fromUtf8(m_mine_sid))));
	m_network_handler->get(m_network_request);
    }
}

void VprotocolWrap::setNewMessagesCount(QString data)
{
    //qDebug()<<"VprotocolWrap::setNewMessagesCount: " << data;
    data.replace("\\/","/");
    data.replace("\\t", " ");

    QScriptEngine sc_engine;
    QScriptValue sc_data = sc_engine.evaluate(data);

    m_new_messages_count = sc_data.property("nm").toInteger();

    if (m_new_messages_count) getNewMessages(m_new_messages_count);
}

void VprotocolWrap::getNewMessages(quint32 count)
{
    //qDebug()<<"VprotocolWrap::getNewMessages: " << count;
    m_network_request.setUrl(QUrl(QString("http://userapi.com/data?act=history&message=305000012&inbox=%1&sid=%2&back=getLastMessages")
				  .arg(count)
				  .arg(QString::fromUtf8(m_mine_sid))));
    m_network_handler->get(m_network_request);
}

void VprotocolWrap::processMessagesArray(QString data)
{
    //qDebug()<<"VprotocolWrap::processMessagesArray: " << data;
    data.replace("\\/","/");
    data.replace("\\t", " ");

    QScriptEngine sc_engine;
    QScriptValue sc_data = sc_engine.evaluate(data);
    QList<Message> qutim_messages;
    QString readed_messages;

    for (quint32 i=0; i<m_new_messages_count; i++) {
	QScriptValue sc_item = sc_data.property("inbox").property("d").property(i);
	readed_messages.append(sc_item.property("0").toString() + "_");
	Message f_mess;
	f_mess.m_time = QDateTime::fromTime_t(sc_item.property("1").toInteger());
	f_mess.m_message = sc_item.property("2").property(0).toString();
	f_mess.m_sender_id = sc_item.property("3").property(0).toString();
	f_mess.m_tmp_name = sc_item.property("3").property(1).toString();
	f_mess.m_tmp_avatar_url = sc_item.property("3").property(2).toString();
	qutim_messages.prepend(f_mess);
	//qDebug() << "processMessagesArray: " << f_mess.m_sender_id << ", " << f_mess.m_message;
    }

    if (qutim_messages.count())	{
	emit getNewMessages(qutim_messages);
	markMessagesAsReaded(readed_messages);
	m_new_messages_count=0;
	//qDebug()<<"processMessagesArray" << "readed_messages: " << readed_messages;
    }
}

void VprotocolWrap::markMessagesAsReaded(QString list)
{
    //qDebug() << "VprotocolWrap::markMessagesAsReaded: " << list;
    m_network_request.setUrl(QUrl(QString("http://userapi.com/data?act=history&read=%1&sid=%2")
				  .arg(list.mid(0,list.length() - 1))
				  .arg(QString::fromUtf8(m_mine_sid))));
    m_network_handler->get(m_network_request);
}

void VprotocolWrap::sendRequestForNews()
{
    if (m_current_state == Online)
    {
	//qDebug() << "VprotocolWrap::sendRequestForNews()";
	QString requrl = QString("http://userapi.com/data?act=history&wall=1&id=%1&sid=%2&back=getNews")
			 .arg(0)
			 .arg(QString::fromUtf8(m_mine_sid));

	if (m_news_photo_enable) requrl += "&updates_photos=1&updates_tagged_photos=1";

	m_network_request.setUrl(QUrl(requrl));
	m_network_handler->get(m_network_request);
    }
}

void VprotocolWrap::processNews(QString data)
{
    //qDebug() << "VprotocolWrap::processNews: " << data;
    data.replace("\\/","/");
    data.replace("\\t", " ");

    QScriptEngine sc_engine;
    QScriptValue sc_data = sc_engine.evaluate(data);

    QList<Photo> qutim_photos;
    QList<Photo> qutim_tagged_photos;

    QString msg_text;

    if (sc_data.property("wall").property("h").isArray()) {
	qDebug()<<"Vkontakte wall update"<<data;
    }
    if (m_news_photo_enable && sc_data.property("updates_tagged_photos").property("h").isArray()) {
	int i=0;
	while (sc_data.property("updates_tagged_photos").property("h").property(i).property(2).isArray()) {
	    QScriptValue sc_photo = sc_data.property("updates_tagged_photos").property("h").property(i).property(2);
	    i++;

	    Photo f_photo;
	    QRegExp rx("^(\\d+)_(\\d+)$");
	    if (rx.indexIn(sc_photo.property(0).toString())==0) {
		f_photo.m_id = rx.cap(1);
		f_photo.m_photo_id = rx.cap(2);
	    }
	    f_photo.m_preview_url = sc_photo.property(1).toString();
	    f_photo.m_photo_url = sc_photo.property(2).toString();

	    if (!m_friend_names[f_photo.m_id].isNull()) {
		msg_text += tr("%1 was tagged on photo").arg(m_friend_names[f_photo.m_id]);
		if (m_news_photo_preview || m_news_photo_fullsize) msg_text += ":";
		msg_text += "\n";
		if (m_news_photo_preview) msg_text += f_photo.m_preview_url+"\n";
		if (m_news_photo_fullsize) msg_text += f_photo.m_photo_url+"\n";
		//		  msg_text += "<br><a href='"+f_photo.m_photo_url+"'><img src='"+f_photo.m_preview_url+"'></a><br>";
	    }
	}
    }
    if (/*m_news_photo_enable &&*/ sc_data.property("updates_photos").property("h").isArray()) {
	int i=0;
	while (sc_data.property("updates_photos").property("h").property(i).property(2).isArray()) {
	    QScriptValue sc_photo = sc_data.property("updates_photos").property("h").property(i).property(2);
	    i++;

	    Photo f_photo;
	    QRegExp rx("^(\\d+)_(\\d+)$");
	    if (rx.indexIn(sc_photo.property(0).toString())==0) {
		f_photo.m_id = rx.cap(1);
		f_photo.m_photo_id = rx.cap(2);
	    }
	    f_photo.m_preview_url = sc_photo.property(1).toString();
	    f_photo.m_photo_url = sc_photo.property(2).toString();

	    if (!m_friend_names[f_photo.m_id].isNull()) {
		msg_text += tr("%1 added new photo").arg(m_friend_names[f_photo.m_id]);
		if (m_news_photo_preview || m_news_photo_fullsize) msg_text += ":";
		msg_text += "\n";
		if (m_news_photo_preview) msg_text += f_photo.m_preview_url+"\n";
		if (m_news_photo_fullsize) msg_text += f_photo.m_photo_url+"\n";
		//		    msg_text += "<br><a href='"+f_photo.m_photo_url+"'><img src='"+f_photo.m_preview_url+"'></a><br>";
	    }
	}
    }

    if (!msg_text.isNull()) {
	Message f_mess;
	f_mess.m_time = QDateTime::currentDateTime();
	f_mess.m_message = msg_text;
	f_mess.m_sender_id = "news";
	QList<Message> qutim_messages;
	qutim_messages.append(f_mess);
	emit getNewMessages(qutim_messages);
    }
}

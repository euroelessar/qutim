///****************************************************************************
// *  notification.h
// *
// *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
// *
// ***************************************************************************
// *                                                                         *
// *   This library is free software; you can redistribute it and/or modify  *
// *   it under the terms of the GNU General Public License as published by  *
// *   the Free Software Foundation; either version 2 of the License, or     *
// *   (at your option) any later version.                                   *
// *                                                                         *
// ***************************************************************************
//*****************************************************************************/

//#ifndef NOTIFICATION_H
//#define NOTIFICATION_H

//#include <QObject>

////Notifications API Draft
//namespace qutim_sdk_0_3 {

//class RemoteAction
//{

//};

//class NotificationBackend;
//class Notification : public QObject
//{
//    Q_OBJECT
//public:
//	enum Type
//	{
//		Invalid = 0
//	};
//	explicit Notification(const QString &body, QObject *sender = 0);
//	explicit Notification(Type type, const QString &body, QObject *sender = 0);
//	void setBody(const QString &body);
//	void setSubject(const QString &summary);
//	void setImagePath(const QString &path);
//	void addAction(RemoteAction);
//	void removeAction(RemoteAction);
//	QList<RemoteAction> actions() const;
//	QString body() const;
//	QString subject() const;
//	Type type() const;
//	QString imagePath() const;
//	void send();
//protected:
//	QObject *object() const;
//	friend class NotificationBackend;
//};

//} // namespace qutim_sdk_0_3

//Q_ENUMS(qutim_sdk_0_3::Notification::Type)

//#endif // NOTIFICATION_H

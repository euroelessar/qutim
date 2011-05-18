/****************************************************************************
 *
 *  This file is part of qutIM
 *
 *  Copyright (c) 2010 by Nigmatullin Ruslan <euroelessar@gmail.com>
 *
 ***************************************************************************
 *                                                                         *
 *   This file is part of free software; you can redistribute it and/or    *
 *   modify it under the terms of the GNU General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************
 ****************************************************************************/

#ifndef BUDDYADAPTER_H
#define BUDDYADAPTER_H

#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>
#include <QDBusConnection>
#include <qutim/buddy.h>

using namespace qutim_sdk_0_3;

class BuddyAdapter : public QDBusAbstractAdaptor
{
	Q_OBJECT
	Q_CLASSINFO("D-Bus Interface", "org.qutim.Buddy")
	Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
	Q_PROPERTY(QString avatar READ avatar NOTIFY avatarChanged)
	Q_PROPERTY(qutim_sdk_0_3::Status status READ status NOTIFY statusChanged)
public:
	explicit BuddyAdapter(Buddy *buddy);
	inline QString name() const { return self()->name(); }
	inline void setName(const QString &n) { self()->setName(n); }
	inline QString avatar() const { return self()->avatar(); }
	inline Status status() const { return self()->status(); }
signals:
	void avatarChanged(const QString &path);
	void statusChanged(const qutim_sdk_0_3::Status &current, const qutim_sdk_0_3::Status &previous);
	void nameChanged(const QString &current, const QString &previous);
private:
	inline Buddy *self() const { return static_cast<Buddy*>(parent()); }
};

#endif // BUDDYADAPTER_H

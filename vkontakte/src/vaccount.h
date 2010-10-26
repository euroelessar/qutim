/****************************************************************************
 *  vaccount.h
 *
 *  Copyright (c) 2010 by Aleksey Sidorov <sauron@citadelspb.com>
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

#ifndef VKONTAKTEACCOUNT_H
#define VKONTAKTEACCOUNT_H
#include <qutim/account.h>
#include "vkontakte_global.h"

class VAccountPrivate;
class VContact;
class VConnection;
class VRoster;
class VWallSession;
class LIBVKONTAKTE_EXPORT VAccount : public Account
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VAccount)
public:
	VAccount(const QString& email,QObject *parent = 0);
	virtual VContact* getContact(const QString& uid, bool create = false);
	virtual ChatUnit* getUnit(const QString& unitId, bool create = false);
	QString uid() const;
	virtual QString name() const;
	void setAccountName(const QString &name);
	QString email() const {return id();} //alias for id
	void setUid(const QString &uid);
	virtual void setStatus(Status status);
	virtual ~VAccount();
	VConnection *connection();
	const VConnection *connection() const;
public slots:
	void loadSettings();
	void saveSettings();
protected:
	QString password();
	virtual bool event(QEvent *ev);
private:
	QScopedPointer<VAccountPrivate> d_ptr;
	friend class VConnection;
	friend class VRosterPrivate;
	friend class VRoster;
};

#endif // VKONTAKTEACCOUNT_H

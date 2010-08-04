/****************************************************************************
 *  vkontakteprotocol_p.h
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

#ifndef VKONTAKTEPROTOCOL_P_H
#define VKONTAKTEPROTOCOL_P_H
#include <QHash>
#include <QPointer>

class VAccount;
class QAction;
class VkontakteProtocol;
class VkontakteProtocolPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(VkontakteProtocol)
public:
	VkontakteProtocolPrivate() :
		accounts_hash(new QHash<QString, QPointer<VAccount> > ())
	{ }
	~VkontakteProtocolPrivate() { delete accounts_hash; }
	union
	{
		QHash<QString, QPointer<VAccount> > *accounts_hash;
		QHash<QString, VAccount *> *accounts;
	};
	VkontakteProtocol *q_ptr;
public slots:
	void onAccountDestroyed(QObject *obj);
	void onOpenWebPageTriggered(QObject *obj);
};

#endif // VKONTAKTEPROTOCOL_P_H

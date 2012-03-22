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

#ifndef VKONTAKTEPROTOCOL_P_H
#define VKONTAKTEPROTOCOL_P_H
#include <QHash>
#include <QWeakPointer>

class VAccount;
class QAction;
class VkontakteProtocol;
class VkontakteProtocolPrivate : public QObject
{
	Q_OBJECT
	Q_DECLARE_PUBLIC(VkontakteProtocol)
public:
	VkontakteProtocolPrivate() :
		accounts_hash(new QHash<QString, QWeakPointer<VAccount> > ())
	{ }
	~VkontakteProtocolPrivate() { delete accounts_hash; }
	union
	{
		QHash<QString, QWeakPointer<VAccount> > *accounts_hash;
		QHash<QString, VAccount *> *accounts;
	};
	VkontakteProtocol *q_ptr;
public slots:
	void onAccountDestroyed(QObject *obj);
	void onOpenWebPageTriggered(QObject *obj);
};

#endif // VKONTAKTEPROTOCOL_P_H


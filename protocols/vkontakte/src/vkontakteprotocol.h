/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef VKONTAKTEPROTOCOL_H
#define VKONTAKTEPROTOCOL_H
#include <qutim/protocol.h>
#include "vkontakte_global.h"
#include <qutim/debug.h>

namespace qutim_sdk_0_3
{
class SettingsItem;
}

class VkontakteProtocolPrivate;
class LIBVKONTAKTE_EXPORT VkontakteProtocol : public Protocol
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(VkontakteProtocol)
	Q_CLASSINFO("Protocol", "vkontakte")
public:
	VkontakteProtocol();
	virtual ~VkontakteProtocol();
	virtual Account* account(const QString& id) const;
	virtual void loadAccounts();
	virtual QList< Account* > accounts() const;
	static inline VkontakteProtocol *instance() { if (!self) warning() << "IcqProtocol isn't created"; return self; }
	QVariant data(DataType type);
protected:
	bool event(QEvent *ev);
private slots:
private:
	static VkontakteProtocol *self;
	QScopedPointer<VkontakteProtocolPrivate> d_ptr;
	friend class VAccountCreator;
	SettingsItem *m_mainSettings;
};

#endif // VKONTAKTEPROTOCOL_H


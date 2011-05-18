/****************************************************************************
 *  factory.h
 *
 *  Copyright (c) 2011 by Sidorov Aleksey <sauron@citadelspb.com>
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

#ifndef FACTORY_H
#define FACTORY_H
#include <qutim/rosterstorage.h>

namespace Core {
namespace MetaContacts {

class Manager;
class Factory : public qutim_sdk_0_3::ContactsFactory
{
public:
	Factory(Manager *manager);
	virtual ~Factory();
	virtual qutim_sdk_0_3::Contact *addContact(const QString &id, const QVariantMap &data);
	virtual void serialize(qutim_sdk_0_3::Contact *contact, QVariantMap &data);
private:
	Manager *m_manager;
};

} // namespace MetaContacts
} // namespace Core

#endif // FACTORY_H

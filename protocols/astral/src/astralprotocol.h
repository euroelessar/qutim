/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Ruslan Nigmatullin euroelessar@yandex.ru
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
#ifndef ASTRALPROTOCOL_H
#define ASTRALPROTOCOL_H

#include <qutim/protocol.h>
#include <qutim/objectgenerator.h>
#include "astralaccount.h"

struct AstralProtocolPrivate;

class AstralProtocol : public Protocol
{
	Q_OBJECT
public:
	AstralProtocol(ConnectionManagerPtr manager, QMetaObject *meta);
	virtual ~AstralProtocol();
	virtual QList<qutim_sdk_0_3::Account *> accounts() const;
	virtual qutim_sdk_0_3::Account *account(const QString &id) const;
	virtual AccountCreationWizard *accountCreationWizard();
	ConnectionManagerPtr connectionManager();
	AccountManagerPtr accountManager();
private:
	virtual void loadAccounts();
	QScopedPointer<AstralProtocolPrivate> p;
};

struct AstralMetaObject : public QMetaObject
{
	AstralMetaObject(ConnectionManagerPtr manager, ProtocolInfo *protocol);
};

class AstralProtocolGenerator : public ObjectGenerator
{
	Q_DISABLE_COPY(AstralProtocolGenerator)
public:
	inline AstralProtocolGenerator(ConnectionManagerPtr m, ProtocolInfo *p)
		: m_manager(m), m_meta(new AstralMetaObject(m,p)) {}
protected:
	virtual QObject *generateHelper() const
	{
		if(m_object.isNull())
			m_object = new AstralProtocol(m_manager, m_meta);
		return m_object.data();
	}
	virtual const QMetaObject *metaObject() const
	{
		return m_meta;
	}
	virtual bool hasInterface(const char *) const
	{
		return false;
	}
private:
	ConnectionManagerPtr m_manager;
	QMetaObject *m_meta;
	mutable QPointer<QObject> m_object;
};

#endif // ASTRALPROTOCOL_H


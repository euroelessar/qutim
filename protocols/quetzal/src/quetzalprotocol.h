/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
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

#ifndef PURPLEPROTOCOL_H
#define PURPLEPROTOCOL_H

#include <qutim/protocol.h>
#include <qutim/plugin.h>
#include <qutim/objectgenerator.h>
#include <purple.h>
#include "quetzalaccountwizard.h"

using namespace qutim_sdk_0_3;

class QuetzalMetaObject;
class QuetzalAccount;
class QuetzalProtocolGenerator;

class QuetzalProtocol : public Protocol
{
    Q_OBJECT
public:
	virtual ~QuetzalProtocol();
    virtual QList<Account *> accounts() const;
	void addAccount(PurpleAccount *account);
    virtual Account *account(const QString &id) const;
	PurplePlugin *plugin() { return m_plugin; }
	virtual QVariant data(DataType type);
	virtual void removeAccount(qutim_sdk_0_3::Account *account, RemoveFlag flags = DeleteAccount);
	static QHash<PurplePlugin *, QuetzalProtocol *> &protocols()
	{
		static QHash<PurplePlugin *, QuetzalProtocol *> protos;
		return protos;
	}
	
protected slots:
	void onAccountRemoved(QObject *object);
	
protected:
	QuetzalProtocol(PurplePlugin *plugin);
	
private:
	void registerAccount(QuetzalAccount *account);
    virtual void loadAccounts();
	PurplePlugin *m_plugin;
	QHash<QString, QuetzalAccount *> m_accounts;
};

class QuetzalProtocolHook : public QuetzalProtocol
{
public:
	QuetzalProtocolHook(const QuetzalMetaObject *meta, PurplePlugin *plugin);
	virtual ~QuetzalProtocolHook();
	
	const QMetaObject *metaObject() const;
	void *qt_metacast(const char *_clname);
private:
	const QuetzalMetaObject *m_metaObject;
};

class QuetzalMetaObject : public QMetaObject
{
public:
	QuetzalMetaObject(PurplePlugin *protocol);
	QuetzalMetaObject(QuetzalProtocolGenerator *protocol);
};

class QuetzalProtocolGenerator : public ObjectGenerator
{
    Q_DISABLE_COPY(QuetzalProtocolGenerator)
public:
	inline QuetzalProtocolGenerator(PurplePlugin *protocol) :
			m_protocol(protocol), m_meta(new QuetzalMetaObject(protocol)) {}
	inline QuetzalProtocolGenerator(QuetzalProtocolGenerator *gen) :
			m_protocol(0), m_meta(new QuetzalMetaObject(gen)) {}
	~QuetzalProtocolGenerator() { delete m_meta; }
	virtual const QMetaObject *metaObject() const
	{
		return m_meta;
	}
	inline PurplePlugin *plugin() const { return m_protocol; }
protected:
	virtual QObject *generateHelper() const
	{
		if(m_object.isNull()) {
			if (m_protocol)
				m_object = new QuetzalProtocolHook(m_meta, m_protocol);
			else
				m_object = new QuetzalAccountWizard(m_meta);
		}
		return m_object.data();
	}
private:
	mutable QWeakPointer<QObject> m_object;
    PurplePlugin *m_protocol;
    const QuetzalMetaObject *m_meta;
};

#endif // PURPLEPROTOCOL_H


/****************************************************************************
 *  quetzalprotocol.h
 *
 *  Copyright (c) 2009 by Nigmatullin Ruslan <euroelessar@gmail.com>
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
	QuetzalProtocol(const QuetzalMetaObject *meta, PurplePlugin *plugin);
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
private:
	void registerAccount(QuetzalAccount *account);
    virtual void loadAccounts();
	PurplePlugin *m_plugin;
	QHash<QString, QuetzalAccount *> m_accounts;
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
				m_object = new QuetzalProtocol(m_meta, m_protocol);
			else
				m_object = new QuetzalAccountWizard(m_meta);
		}
		return m_object.data();
	}
private:
	mutable QPointer<QObject> m_object;
    PurplePlugin *m_protocol;
    const QuetzalMetaObject *m_meta;
};

#endif // PURPLEPROTOCOL_H

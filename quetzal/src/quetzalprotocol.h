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

using namespace qutim_sdk_0_3;

class QuetzalMetaObject;
class QuetzalAccount;

class QuetzalProtocol : public Protocol
{
    Q_OBJECT
public:
	QuetzalProtocol(const QuetzalMetaObject *meta, PurplePlugin *plugin);
    virtual QList<Account *> accounts() const;
    virtual Account *account(const QString &id) const;
	PurplePlugin *plugin() { return m_plugin; }
	static QHash<PurplePlugin *, QuetzalProtocol *> &protocols()
	{
		static QHash<PurplePlugin *, QuetzalProtocol *> protos;
		return protos;
	}
private:
    virtual void loadAccounts();
	PurplePlugin *m_plugin;
	QHash<QString, QuetzalAccount *> m_accounts;
};

class QuetzalMetaObject : public QMetaObject
{
public:
    QuetzalMetaObject(PurplePlugin *protocol);
};

class QuetzalProtocolGenerator : public ObjectGenerator
{
    Q_DISABLE_COPY(QuetzalProtocolGenerator)
public:
    inline QuetzalProtocolGenerator(PurplePlugin *protocol)
        : m_protocol(protocol), m_meta(new QuetzalMetaObject(protocol)) {}
protected:
	virtual QObject *generateHelper() const
	{
		if(m_object.isNull())
			m_object = new QuetzalProtocol(m_meta, m_protocol);
		return m_object.data();
	}
	virtual const QMetaObject *metaObject() const
	{
		return m_meta;
	}
	virtual const char *iid() const
	{
		return 0;
	}
	virtual bool hasInterface(const char *id) const
	{
		Q_UNUSED(id);
		return false;
	}
private:
    mutable QPointer<QuetzalProtocol> m_object;
    PurplePlugin *m_protocol;
    const QuetzalMetaObject *m_meta;
};

#endif // PURPLEPROTOCOL_H

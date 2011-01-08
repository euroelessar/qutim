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

#ifndef PURPLEPROTOCOL_H
#define PURPLEPROTOCOL_H

#include <qutim/protocol.h>
#include <qutim/plugin.h>
#include <qutim/objectgenerator.h>
#include <purple.h>

using namespace qutim_sdk_0_3;

class QuetzalMetaObject;

class QuetzalProtocol : public Protocol
{
    Q_OBJECT
public:
    QuetzalProtocol(const QuetzalMetaObject *meta);
    virtual QList<Account *> accounts() const;
    virtual Account *account(const QString &id) const;
private:
    virtual void loadAccounts();
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
            m_object = new QuetzalProtocol(m_meta);
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

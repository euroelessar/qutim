#include "astralprotocol.h"

struct AstralProtocolPrivate
{
	ConnectionManagerPtr conn_mgr;
	AccountManagerPtr acc_mgr;
	QHash<QString, QPointer<AstralAccount> > accounts;
};

AstralProtocol::AstralProtocol(ConnectionManagerPtr manager, QMetaObject *meta) : p(new AstralProtocolPrivate)
{
	p->conn_mgr = manager;
	QObject::d_ptr->metaObject = meta;
}

AstralProtocol::~AstralProtocol()
{
	QObject::d_ptr->metaObject = 0;
}

QList<qutim_sdk_0_3::Account *> AstralProtocol::accounts() const
{
	QList<qutim_sdk_0_3::Account*> accounts;
	foreach(qutim_sdk_0_3::Account *acc, p->accounts)
	{
		if(acc)
			accounts << acc;
	}
	return accounts;
}

qutim_sdk_0_3::Account *AstralProtocol::account(const QString &id) const
{
	return p->accounts.value(id);
}

AccountCreationWizard *AstralProtocol::accountCreationWizard()
{
	return 0;
}

ConnectionManagerPtr AstralProtocol::connectionManager()
{
	return p->conn_mgr;
}

AccountManagerPtr AstralProtocol::accountManager()
{
	return p->acc_mgr;
}

void AstralProtocol::loadAccounts()
{
	QStringList accounts = config("general").value("accounts", QStringList());
	foreach(const QString &id, accounts)
		p->accounts.insert(id, new AstralAccount(this, id));
}

AstralMetaObject::AstralMetaObject(ConnectionManagerPtr manager, ProtocolInfo *protocol)
{
	QByteArray stringdata_b = "astral::";
	stringdata_b += manager->name().toLatin1();
	stringdata_b += "::";
	stringdata_b += protocol->name().replace('-', '_').toLatin1();
	stringdata_b += '\0';
	int value = stringdata_b.size();
	stringdata_b += protocol->name().toLatin1();
	stringdata_b += '\0';
	int key = stringdata_b.size();
	stringdata_b.append("Protocol\0", 9);

	char *stringdata = (char*)qMalloc(stringdata_b.size() + 1);
	uint *data = (uint*) calloc(17, sizeof(uint));
	qMemCopy(stringdata, stringdata_b.constData(), stringdata_b.size() + 1);
	data[0] = 4;
	data[2] = 1;
	data[3] = 14;
	data[14] = key;
	data[15] = value;

	d.superdata = &AstralProtocol::staticMetaObject;
	d.stringdata = stringdata;
	d.data = data;
	d.extradata = 0;
}

#include "quetzalprotocol.h"

QuetzalProtocol::QuetzalProtocol(const QuetzalMetaObject *meta)
{
    QObject::d_ptr->metaObject = const_cast<QuetzalMetaObject *>(meta);
}

QList<Account *> QuetzalProtocol::accounts() const
{
    return QList<Account *>();
}

Account *QuetzalProtocol::account(const QString &id) const
{
    return 0;
}

void QuetzalProtocol::loadAccounts()
{
}

//static const uint qt_meta_data_PurpleSubProtocol[] = {
//
// // content:
//	   2,       // revision
//	   0,       // classname
//	   0,    0, // classinfo
//	   0,    0, // methods
//	   0,    0, // properties
//	   0,    0, // enums/sets
//	   0,    0, // constructors
//
//	   0        // eod
//};
//
//static const char qt_meta_stringdata_PurpleProtocolPlugin[] = {
//	"PurpleProtocolPlugin\0"
//};

//const QMetaObject QuetzalProtocol::staticMetaObject = {
//	{ &Plugin::staticMetaObject, qt_meta_stringdata_PurpleProtocolPlugin,
//      qt_meta_data_PurpleSubProtocol, 0 }
//};

QuetzalMetaObject::QuetzalMetaObject(PurplePlugin *protocol)
{
	QByteArray stringdata_b = "Quetzal::";
	stringdata_b += protocol->info->id;
	stringdata_b += '\0';
	int value = stringdata_b.size();
	stringdata_b += protocol->info->name;
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

	d.superdata = &QuetzalProtocol::staticMetaObject;
	d.stringdata = stringdata;
	d.data = data;
	d.extradata = 0;
}

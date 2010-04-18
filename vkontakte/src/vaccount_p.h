#ifndef VACCOUNT_P_H
#define VACCOUNT_P_H
#include <QList>
#include <QHash>

class VConnection;
class VContact;
struct VAccountPrivate
{
	QString name;
	QString uid;
	QHash<QString, VContact*> contactsHash;
	QList<VContact *> contactsList;
	VConnection *connection;
};

#endif // VACCOUNT_P_H

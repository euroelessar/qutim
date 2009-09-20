#include <QtCore/QCoreApplication>
#include <QDataStream>
#include <QDebug>
#include "tlv.h"
#include "oscarconnection.h"
#include <QCryptographicHash>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	OscarConnection conn;
	conn.connectToLoginServer();
    return a.exec();
}

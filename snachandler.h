#ifndef SNACHANDLER_H
#define SNACHANDLER_H

#include <QObject>
#include <QPair>
#include "snac.h"
#include "util.h"
#include "oscarconnection.h"

typedef QPair<quint16, quint16> SNACInfo;

class SNACHandler : public QObject
{
	Q_OBJECT
public:
    SNACHandler();
	const QList<SNACInfo> &infos() { return m_infos; }
	virtual void handleSNAC(OscarConnection *conn, const SNAC &snac) = 0;
protected:
	QList<SNACInfo> m_infos;
};

#endif // SNACHANDLER_H

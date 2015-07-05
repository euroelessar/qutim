#ifndef KDEPASTER_H
#define KDEPASTER_H

#include "pasterinterface.h"

class KdePaster : public PasterInterface
{
public:
	KdePaster();

	virtual QString name();
	virtual QNetworkReply *send(QNetworkAccessManager *manager, const QString &content, const QString &syntax);
	virtual QUrl handle(QNetworkReply *reply, QString *error);
};

#endif // KDEPASTER_H

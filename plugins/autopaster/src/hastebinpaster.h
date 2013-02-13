#ifndef HASTEBINPASTER_H
#define HASTEBINPASTER_H

#include "pasterinterface.h"

class HastebinPaster : public PasterInterface
{
public:
	HastebinPaster();

	virtual QString name();
	virtual QNetworkReply *send(QNetworkAccessManager *manager, const QString &content, const QString &syntax);
	virtual QUrl handle(QNetworkReply *reply, QString *error);
};

#endif // HASTEBINPASTER_H

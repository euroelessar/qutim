#ifndef UBUNTUPASTER_H
#define UBUNTUPASTER_H

#include "pasterinterface.h"

class UbuntuPaster : public PasterInterface
{
public:
	UbuntuPaster();

	virtual QString name();
	virtual QNetworkReply *send(QNetworkAccessManager *manager, const QString &content, const QString &syntax);
	virtual QUrl handle(QNetworkReply *reply, QString *error);

	void appendPart(QHttpMultiPart *multi, const QByteArray &name, const QByteArray &value);
};

#endif // UBUNTUPASTER_H

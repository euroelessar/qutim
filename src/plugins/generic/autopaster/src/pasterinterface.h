#ifndef PASTERINTERFACE_H
#define PASTERINTERFACE_H

#include <QMetaType>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class PasterInterface
{
public:
	virtual ~PasterInterface() {}

	virtual QString name() = 0;
	virtual QNetworkReply *send(QNetworkAccessManager *manager, const QString &content, const QString &syntax) = 0;
	virtual QUrl handle(QNetworkReply *reply, QString *error) = 0;
};

Q_DECLARE_METATYPE(PasterInterface*)

#endif // PASTERINTERFACE_H

#ifndef VREQUEST_H
#define VREQUEST_H

#include <QNetworkRequest>
#include "vkontakte_global.h"

class VRequest : public QNetworkRequest
{
public:
	VRequest(const QUrl& url = QUrl());
};

#endif // VREQUEST_H

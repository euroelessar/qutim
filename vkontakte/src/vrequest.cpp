#include "vrequest.h"

VRequest::VRequest(const QUrl& url): QNetworkRequest(url)
{
    setRawHeader("User-Agent", "qutIM plugin VKontakte");
    setRawHeader("Accept-Charset", "utf-8");
    setRawHeader("Pragma", "no-cache");
    setRawHeader("Cache-control", "no-cache");
}

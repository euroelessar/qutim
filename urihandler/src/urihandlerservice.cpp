#include "urihandlerservice.h"
#include "handler_xmpp.h"

UriHandlerService *instance = 0;

void UriHandlerService::open(const QString &uriString)
{
	QUrl uri(uriString);
	AbstractHandler *h = handlers_[uri.scheme()];
	if (h)
		h->open(uri);
}

UriHandlerService::UriHandlerService()
{
	instance = this;
}

UriHandlerService::~UriHandlerService()
{
	qDeleteAll(handlers_.values());
	handlers_.clear();
	instance = 0;
}

bool UriHandlerService::addHandlerImpl(const QString &scheme, AbstractHandler *h)
{
	if (instance) {
		instance->handlers_[scheme] = h;
		return true;
	} else
		return false;
}

bool UriHandlerService::removeHandler(const QString &scheme)
{
	if (instance) {
		delete instance->handlers_[scheme];
		instance->handlers_.remove(scheme);
		return true;
	} else
		return false;
}

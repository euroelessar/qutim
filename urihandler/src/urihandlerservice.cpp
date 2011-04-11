#include "urihandlerservice.h"
#include "handler_xmpp.h"

static UriHandlerService *instance = 0;

void UriHandlerService::open(const QString &uriString)
{
	QUrl uri(uriString);
	AbstractHandler *h = m_handlers[uri.scheme()];
	if (h)
		h->open(uri);
}

UriHandlerService::UriHandlerService()
{
	instance = this;
}

UriHandlerService::~UriHandlerService()
{
	qDeleteAll(m_handlers.values());
	m_handlers.clear();
	instance = 0;
}

bool UriHandlerService::addHandlerImpl(const QString &scheme, AbstractHandler *h)
{
	if (instance) {
		instance->m_handlers[scheme] = h;
		return true;
	} else
		return false;
}

bool UriHandlerService::removeHandler(const QString &scheme)
{
	if (instance) {
		delete instance->m_handlers[scheme];
		instance->m_handlers.remove(scheme);
		return true;
	} else
		return false;
}

#ifndef URIHANDLERSERVICE_H
#define URIHANDLERSERVICE_H

#include <QMap>

#include "abstract_handler.h"

#ifdef urihandler_EXPORTS
#	define UHSERVICE_EXPORTS Q_DECL_EXPORT
#else
#	define UHSERVICE_EXPORTS Q_DECL_IMPORT
#endif

class UHSERVICE_EXPORTS UriHandlerService : public QObject
{
	Q_OBJECT
	Q_CLASSINFO("Service", "UriHandler")
	Q_CLASSINFO("Uses",    "ChatLayer")
public:
	UriHandlerService();
	~UriHandlerService();
	Q_INVOKABLE void open(const QString &uri);
	template <class T>
	static bool addHandler(const QString &scheme)
	{
		return addHandlerImpl(scheme, new T);
	}
	static bool removeHandler(const QString &scheme);

private:
	typedef QMap<QString, AbstractHandler*> HandlersMap;
	HandlersMap m_handlers;
	static bool addHandlerImpl(const QString &scheme, AbstractHandler *h);
};

#endif // URIHANDLERSERVICE_H

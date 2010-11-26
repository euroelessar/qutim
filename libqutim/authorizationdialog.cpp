#include "authorizationdialog.h"
#include "objectgenerator.h"
#include "servicemanager.h"

namespace qutim_sdk_0_3
{

QObject *service()
{
	return ServiceManager::getByName("AuthorizationService");
}

QEvent::Type AuthorizationRequest::eventType()
{
	static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 103));
	return type;
}

QEvent::Type AuthorizationReply::eventType()
{
	static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 104));
	return type;
}

}

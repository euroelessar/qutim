#include "authorizationdialog.h"
#include "objectgenerator.h"
#include "servicemanager.h"

namespace qutim_sdk_0_3
{

namespace Authorization
{

QObject *service()
{
	return ServiceManager::getByName("AuthorizationService");
}

QEvent::Type Request::eventType()
{
	static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 103));
	return type;
}

QEvent::Type Reply::eventType()
{
	static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 104));
	return type;
}

}//namespace Authorization

}//namespace qutim_sdk_0_3


#include "authorizationdialog.h"
#include "objectgenerator.h"

namespace qutim_sdk_0_3
{

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

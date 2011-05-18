#include "inforequest.h"

namespace qutim_sdk_0_3
{
	InfoRequest::InfoRequest()
	{
	}

	InfoRequest::~InfoRequest()
	{
	}

	void InfoRequest::resend() const
	{
	}

	void InfoRequest::virtual_hook(int id, void *data)
	{
		Q_UNUSED(id);
		Q_UNUSED(data);
	}

	InfoRequestCheckSupportEvent::InfoRequestCheckSupportEvent(SupportType type) :
		QEvent(eventType()), m_supportType(type)
	{
	}

	QEvent::Type InfoRequestCheckSupportEvent::eventType()
	{
		static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 110));
		return type;
	}

	InfoRequestEvent::InfoRequestEvent() :
		QEvent(eventType()), m_request(0)
	{
	}

	QEvent::Type InfoRequestEvent::eventType()
	{
		static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 111));
		return type;
	}

	InfoItemUpdatedEvent::InfoItemUpdatedEvent(const DataItem &newInfoItem) :
		QEvent(eventType()), m_info(newInfoItem)
	{
	}

	QEvent::Type InfoItemUpdatedEvent::eventType()
	{
		static QEvent::Type type = QEvent::Type(QEvent::registerEventType(QEvent::User + 112));
		return type;
	}
}

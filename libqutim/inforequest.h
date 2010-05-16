#ifndef INFOREQUEST_H
#define INFOREQUEST_H

#include "dataforms.h"
#include <QEvent>

namespace qutim_sdk_0_3
{
	typedef DataItem InfoItem;

	class LIBQUTIM_EXPORT InfoRequest : public QObject
	{
		Q_OBJECT
		Q_DISABLE_COPY(InfoRequest)
	public:
		enum State {
			Request,
			Done,
			Cancel,
			Cache
		};
		InfoRequest();
		virtual ~InfoRequest();
		virtual DataItem item(const QString &name = QString()) const = 0;
		virtual State state() const = 0;
		virtual void resend() const;
	signals:
		void stateChanged(InfoRequest::State state);
	protected:
		virtual void virtual_hook(int id, void *data);
	};

	class LIBQUTIM_EXPORT InfoRequestCheckSupportEvent : public QEvent
	{
	public:
		enum SupportType {
			NoSupport,
			Read,
			ReadWrite
		};
		InfoRequestCheckSupportEvent(SupportType type = NoSupport);
		static QEvent::Type eventType();
		SupportType supportType() { return m_supportType; }
		void setSupportType(SupportType supportType) { m_supportType = supportType; }
	private:
		SupportType m_supportType;
	};

	class LIBQUTIM_EXPORT InfoRequestEvent : public QEvent
	{
	public:
		InfoRequestEvent();
		static QEvent::Type eventType();
		InfoRequest *request() { return m_request; }
		void setRequest(InfoRequest *request) { Q_ASSERT(!m_request); m_request = request; }
	private:
		InfoRequest *m_request;
	};

	class LIBQUTIM_EXPORT InfoItemUpdatedEvent : public QEvent
	{
	public:
		InfoItemUpdatedEvent(const DataItem &newInfoItem);
		static QEvent::Type eventType();
		DataItem infoItem() { return m_info; };
	private:
		DataItem m_info;
	};
}

#endif // INFOREQUEST_H

#ifndef AUTHORIZATIONDIALOG_H
#define AUTHORIZATIONDIALOG_H

#include "libqutim_global.h"
#include <QEvent>

namespace qutim_sdk_0_3
{
class Contact;
/**
* Send AuthorizationRequest to @param contact
*/
class LIBQUTIM_EXPORT AuthorizationRequest  : public QEvent
{
public:
	AuthorizationRequest(Contact *contact,const QString &body = QString())
		:	QEvent(eventType()),m_contact(contact),m_body(body) {}
	Contact *contact() const {return m_contact;}
	QString body() const {return m_body;}
	/** @reimp */
	static QEvent::Type eventType();
private:
	Type m_type;
	Contact *m_contact;
	QString m_body;
};

class LIBQUTIM_EXPORT AuthorizationReply  : public QEvent
{
public:
	enum Type {
		New,
		Accept,
		Reject,
		Accepted,
		Rejected
	};
	AuthorizationReply(Type type, Contact *contact,const QString &body = QString())
		:	QEvent(eventType()),m_type(type),m_contact(contact),m_body(body) {}
	Contact *contact() const {return m_contact;}
	QString body() const {return m_body;}
	Type replyType() const {return m_type;}
	void setType(Type type) {m_type = type;}
	/** @reimp */
	static QEvent::Type eventType();
private:
	Type m_type;
	Contact *m_contact;
	QString m_body;
};

}

#endif // AUTHORIZATIONDIALOG_H

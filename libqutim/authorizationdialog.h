#ifndef AUTHORIZATIONDIALOG_H
#define AUTHORIZATIONDIALOG_H

#include "libqutim_global.h"
#include <QEvent>

namespace qutim_sdk_0_3
{
class Contact;
class AuthorizationDialogPrivate;

class LIBQUTIM_EXPORT AuthorizationDialog : public QObject
{

	Q_OBJECT
	Q_DISABLE_COPY(AuthorizationDialog)
public:
	virtual ~AuthorizationDialog();
	static AuthorizationDialog *request(Contact *contact, const QString &text, bool incoming = true);
	Contact *contact() const;
	virtual QString text() const = 0;
protected:
	explicit AuthorizationDialog();
	virtual void setContact(Contact *contact, const QString &text, bool incoming = true) = 0;
	virtual void virtual_hook(int id, void *data);
	QScopedPointer<AuthorizationDialogPrivate> d;
signals:
	void accepted();
	void rejected();
	void finished(bool ok);
};

//	class LIBQUTIM_EXPORT AuthorizationManager : public QObject
//	{
//		Q_OBJECT
//		Q_CLASSINFO("Service", "AuthorizationDialog")
//	public:
//		static AuthorizationManager *instance();
//		virtual AuthorizationDialog *request(Contact *contact, const QString &text) = 0;
//		virtual void virtual_hook(int id, void *data);
//	protected:
//		AuthorizationManager();
//		~AuthorizationManager();
//	};

//new draft, based on events
class LIBQUTIM_EXPORT AuthEvent  : public QEvent
{
public:
	enum Type {Request,
			   Recieved,
			   Accept,
			   Reject};
	AuthEvent(Type type,Contact *contact,const QString &message = QString())
		:	QEvent(eventType()),m_type(type),m_contact(contact),m_message(message) {}
	Type type() const;
	Contact *contact() const;
	QString message() const;
	/**
  * @brief event type
  *
  * @return QEvent::Type eventType
  */
	static QEvent::Type eventType();
private:
	Type m_type;
	Contact *m_contact;
	QString m_message;
};

}

#endif // AUTHORIZATIONDIALOG_H

#ifndef JABBER_SDK_0_3_H
#define JABBER_SDK_0_3_H

#include <QtCore/QObject>
#include <QtCore/QVariantMap>
#include <QtCore/QSharedPointer>

namespace Jreen
{
class Client;
class Payload;
namespace PubSub { class Manager; }
}
namespace gloox
{
class Tag;
class Adhoc;
class VCardManager;
class MessageFilter;
class MessageSession;
}

namespace qutim_sdk_0_3
{
class Account;
}

namespace Jabber
{
/*!
   JabberExtension is abstract interface for every extension
   to Jabber plugin.
 */
class JabberExtension
{
public:
	/*!
	Constructor
  */
	JabberExtension() {}
	/*!
	Destructor
  */
	virtual ~JabberExtension() {}
	/*!
	Initialization method for extension.
	\param account Pointer to jabber account
	\param params Collection of different parameters, such as gloox::Client

	\sa JabberParams
  */
	virtual void init(qutim_sdk_0_3::Account *account) = 0;
};

/*!
   See JPersonMoodConverter as example
 */
class PersonEventConverter
{
public:
	virtual ~PersonEventConverter() {}
	
	virtual QString name() const = 0;
	virtual int entityType() const = 0;
	virtual QSharedPointer<Jreen::Payload> convertTo(const QVariantHash &map) const = 0;
	virtual QVariantHash convertFrom(const QSharedPointer<Jreen::Payload> &entity) const = 0;
};

}

template<typename T>
T qobject_cast(const QVariant &item)
{
    QObject *o = item.value<QObject*>();
    return qobject_cast<T>(o);
}

Q_DECLARE_INTERFACE(Jabber::JabberExtension,		"org.qutim.jabber.JabberExtension")
Q_DECLARE_INTERFACE(Jabber::PersonEventConverter,	"org.qutim.jabber.PersonEventConverter")

#endif // JABBER_SDK_0_3_H

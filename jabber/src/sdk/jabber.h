#ifndef JABBER_SDK_0_3_H
#define JABBER_SDK_0_3_H

#include <QtCore/QObject>
#include <QtCore/QVariantMap>

namespace jreen
{
class Client;
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
   JabberParams is collection for flexible and safe providing of
   pointers to different Jabber objects.

   One of main advantages of this way providing pointers is full
   binary compatibility, it is preserved as much as it let to do
   binary compatibility of gloox library.

   \code
  void MyExtension::init(qutim_sdk_0_3::Account *account, const JabberParams &params)
  {
   gloox::Client *client = params.item<gloox::Client>();
   client->doStuff();
  }
   \endcode
 */
class JabberParams
{
public:
	JabberParams() {}
	~JabberParams() {}
	template<typename T> void addItem(T *item);
	template<typename T> T *item() const;
private:
	QMap<QByteArray, void *> m_params;
};

template<typename T>
Q_INLINE_TEMPLATE void JabberParams::addItem(T *item)
{
	const char *iid = qobject_interface_iid<T *>();
	if(iid && *iid)
		m_params.insert(iid, item);
}

template<typename T>
Q_INLINE_TEMPLATE T *JabberParams::item() const
{
	const char *iid = qobject_interface_iid<T *>();
	if(iid && *iid)
		return reinterpret_cast<T *>(m_params.value(iid, 0));
	return 0;
}
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
	virtual void init(qutim_sdk_0_3::Account *account, const JabberParams &params) = 0;
};

class MessageFilterFactory
{
public:
	MessageFilterFactory() {}
	virtual ~MessageFilterFactory() {}

	virtual gloox::MessageFilter *create(qutim_sdk_0_3::Account *account,
										 const JabberParams &params,
										 gloox::MessageSession *session) = 0;
};

/*!
   See JPersonMoodConverter as example
 */
class PersonEventConverter
{
public:
	virtual std::string feature() const = 0;
	virtual QString name() const = 0;
	virtual gloox::Tag *toXml(const QVariantHash &map) const = 0;
	virtual QVariantHash fromXml(gloox::Tag *tag) const = 0;
	virtual ~PersonEventConverter() {}
};
}

Q_DECLARE_INTERFACE(Jabber::JabberExtension,		"org.qutim.jabber.JabberExtension")
Q_DECLARE_INTERFACE(Jabber::MessageFilterFactory,	"org.qutim.jabber.MessageFilterFactory")
Q_DECLARE_INTERFACE(Jabber::PersonEventConverter,	"org.qutim.jabber.PersonEventConverter")
Q_DECLARE_INTERFACE(jreen::Client,			"org.qutim.jreen.Client")
Q_DECLARE_INTERFACE(gloox::Adhoc,			"net.camaya.gloox.Adhoc")
Q_DECLARE_INTERFACE(gloox::VCardManager,	"net.camaya.gloox.VCardManager")

#endif // JABBER_SDK_0_3_H

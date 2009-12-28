#ifndef JABBER_SDK_0_3_H
#define JABBER_SDK_0_3_H

#include <QtCore/QObject>
#include <QtCore/QMap>

namespace gloox { class Client; class Adhoc; }
namespace qutim_sdk_0_3 { class Account; }

namespace Jabber
{
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

	class JabberExtension
	{
	public:
		JabberExtension() {}
		virtual ~JabberExtension() {}
		virtual void init(qutim_sdk_0_3::Account *account, const JabberParams &params) = 0;
	};
}

Q_DECLARE_INTERFACE(Jabber::JabberExtension, "org.qutim.jabber.JabberExtension")
Q_DECLARE_INTERFACE(gloox::Client, "net.camaya.gloox.Client")
Q_DECLARE_INTERFACE(gloox::Adhoc,  "net.camaya.gloox.Adhoc")

#endif // JABBER_SDK_0_3_H

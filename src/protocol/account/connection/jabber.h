#ifndef JABBER_SDK_0_3_H
#define JABBER_SDK_0_3_H

#include <QtCore/QObject>

namespace gloox { class Client; }
namespace qutim_sdk_0_3 { class Account; }

namespace Jabber
{
	class JabberExtension
	{
	public:
		JabberExtension() {}
		virtual ~JabberExtension() {}
		virtual void setClient(qutim_sdk_0_3::Account *account, gloox::Client *client) = 0;
	};
}

Q_DECLARE_INTERFACE(Jabber::JabberExtension, "org.qutim.jabber.JabberExtension")

#endif // JABBER_SDK_0_3_H

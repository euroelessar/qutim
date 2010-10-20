#ifndef JSERVERDISCOINFO_H
#define JSERVERDISCOINFO_H

#include <gloox/connectionlistener.h>
#include <gloox/discohandler.h>
#include <QObject>

namespace Jabber {

	using namespace gloox;

	class JAccount;

	class JServerDiscoInfo: public QObject,
							public ConnectionListener,
							public DiscoHandler
	{
		Q_OBJECT
	public:
		JServerDiscoInfo(JAccount *account);
		virtual ~JServerDiscoInfo();
		// ConnectionListener
		virtual void onConnect();
		virtual void onDisconnect(ConnectionError error);
		virtual bool onTLSConnect(const CertInfo &) { return true; }
		// DiscoHandler
		enum Contexts
		{
			ServerDiscoInfo
		};
		virtual void handleDiscoInfo(const JID &from, const Disco::Info &info, int context);
		virtual void handleDiscoItems(const JID &from, const Disco::Items &items, int context);
		virtual void handleDiscoError(const JID &from, const Error *error, int context);
	private:
		JAccount *m_account;
	};

} // namespace Jabber

#endif // JSERVERDISCOINFO_H

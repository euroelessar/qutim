#include "jconnectionlistener.h"
#include <QDebug>

namespace Jabber
{
	struct JConnectionListenerPrivate
	{
		JAccount *account;
	};

	JConnectionListener::JConnectionListener(JAccount *account) : p(new JConnectionListenerPrivate), ConnectionListener()
	{
		p->account = account;
		p->account->connection()->client()->registerConnectionListener(this);
	}

	JConnectionListener::~JConnectionListener()
	{
	}

	void JConnectionListener::onConnect()
	{
	}

	void JConnectionListener::onDisconnect(ConnectionError error)
	{
	}

	void JConnectionListener::onResourceBind(const std::string &resource)
	{
	}

	void JConnectionListener::onResourceBindError(const Error *error)
	{
	}

	void JConnectionListener::onSessionCreateError(const Error *error)
	{
	}

	bool JConnectionListener::onTLSConnect(const CertInfo &info)
	{
		Config c = Config(p->account->protocol()->id() + QLatin1Char('.') + p->account->id() + QLatin1String("/certificates"));
		QString s = QString::fromStdString(info.server);

		if (c.hasGroup(s))
			return c.value(s, true);

		if (!p->account->config().group("connect").value("viewCertInfo", false))
			return true;

		bool r;
		if (JCertInfo(info).exec(r)) {
			c.setValue(s, r);
			c.sync();
		}

		if (r)
			return true;
		else
			return false;
	}

	void JConnectionListener::onStreamEvent(StreamEvent event)
	{
	}
}

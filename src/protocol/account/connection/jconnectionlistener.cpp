#include "jconnectionlistener.h"
#include <QDebug>

namespace Jabber
{
	struct JConnectionListenerPrivate
	{
	};

	JConnectionListener::JConnectionListener(JAccount *account) : p(new JConnectionListenerPrivate), ConnectionListener()
	{
		account->connection()->client()->registerConnectionListener(this);
	}

	JConnectionListener::~JConnectionListener()
	{
	}

	void JConnectionListener::onConnect()
	{
		qDebug() << Q_FUNC_INFO;
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
		if (JCertInfo(info).exec())
			return true;
		else
			return false;
	}

	void JConnectionListener::onStreamEvent(StreamEvent event)
	{
	}
}

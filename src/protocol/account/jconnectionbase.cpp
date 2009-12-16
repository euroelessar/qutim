#include "jconnectionbase.h"
#include <QDebug>

namespace Jabber
{
	struct JConnectionBasePrivate
	{
	};

	JConnectionBase::JConnectionBase(ConnectionDataHandler *cdh) : p(new JConnectionBasePrivate), ConnectionBase(cdh)
	{
	}

	JConnectionBase::~JConnectionBase()
	{
	}

	ConnectionError JConnectionBase::connect()
	{
		qDebug() << Q_FUNC_INFO;
	}

	ConnectionError JConnectionBase::recv(int timeout)
	{
	}

	bool JConnectionBase::send(const std::string &data)
	{
	}

	ConnectionError JConnectionBase::receive()
	{
	}

	void JConnectionBase::disconnect()
	{
	}

	void JConnectionBase::cleanup()
	{
	}

	ConnectionState JConnectionBase::state() const
	{
	}

	void JConnectionBase::registerConnectionDataHandler(ConnectionDataHandler *cdh)
	{
	}

	void JConnectionBase::setServer(const std::string &server, int port)
	{
	}

	const std::string &JConnectionBase::server() const
	{
	}

	int JConnectionBase::port() const
	{
	}

	int JConnectionBase::localPort() const
	{
	}

	const std::string JConnectionBase::localInterface() const
	{
	}

	void JConnectionBase::getStatistics(long int &totalIn, long int &totalOut)
	{
	}

	ConnectionBase *JConnectionBase::newInstance() const
	{
	}

}

#include "tcpsocket.h"

namespace qutim_sdk_0_3
{
	struct NetworkProxy
	{
	};

	TcpSocket::TcpSocket(QObject *parent) : QTcpSocket(parent), p(0)
	{
	}

	TcpSocket::~TcpSocket()
	{
	//	delete p;
	}

	void TcpSocket::setProxy(const NetworkProxy &networkProxy)
	{
	}

	NetworkProxy TcpSocket::proxy() const
	{
		return NetworkProxy();
	}

	void TcpSocket::connectToHostImplementation(const QString &hostName, quint16 port, OpenMode mode)
	{
	}

	void TcpSocket::disconnectFromHostImplementation()
	{
	}
}

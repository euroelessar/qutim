#include "accountserver.h"
#include <QTimer>

namespace Bearer {

using namespace qutim_sdk_0_3;

AccountServer::AccountServer(qutim_sdk_0_3::Account *account)
	: QObject(account), m_account(account), m_online(false)
{
	Config config = account->config();
	Status status = config.value("lastStatus", Status(Status::Online));

	qDebug() << account->id() << "is created with status" << status;

	m_timer.setInterval(5 * 1000);
	connect(&m_timer, &QTimer::timeout, this, &AccountServer::updateState);

	account->setUserStatus(status);

	connect(account, &Account::stateChanged, this, [this, account] (Account::State state) {
		if (state == Account::Disconnected && m_online && !m_timer.isActive())
			m_timer.start();
		else if (state == Account::Connected && m_timer.isActive())
			m_timer.stop();

		updateState();
	});
}

void AccountServer::setOnline(bool online)
{
	qDebug() << m_account->id() << "online:" << online;

	m_online = online;

	updateState();

	if (m_online && isDisconnected() && !m_timer.isActive())
		m_timer.start();
}

bool AccountServer::wantOnline() const
{
	return m_account->userStatus() != Status::Offline;
}

bool AccountServer::isConnected() const
{
	return m_account->state() == Account::Connected;
}

bool AccountServer::isDisconnected() const
{
	return m_account->state() == Account::Disconnected;
}

void AccountServer::updateState()
{
	if (m_online && wantOnline() && isDisconnected())
		m_account->connectToServer();
	else if (!m_online && isConnected())
		m_account->disconnectFromServer();
}

} // namespace Bearer

#ifndef BEARER_ACCOUNTSERVER_H
#define BEARER_ACCOUNTSERVER_H

#include <qutim/account.h>
#include <QTimer>

namespace Bearer {

class AccountServer : public QObject
{
	Q_OBJECT
public:
	explicit AccountServer(qutim_sdk_0_3::Account *account);

	void setOnline(bool isOnline);

protected:
	bool wantOnline() const;
	bool isConnected() const;
	bool isDisconnected() const;
	void updateState();

private:
	QTimer m_timer;
	qutim_sdk_0_3::Account *m_account;
	bool m_online;
};

} // namespace Bearer

#endif // BEARER_ACCOUNTSERVER_H

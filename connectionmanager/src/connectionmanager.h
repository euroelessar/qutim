#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <qutim/plugin.h>
#include <qutim/settingslayer.h>
#include <qutim/status.h>
#include <QQueue>

#if (QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))
#	define HAVE_NETWORK_MANAGER
#endif

namespace qutim_sdk_0_3
{
	class Account;
}

class QNetworkConfigurationManager;
namespace ConnectionManager
{
	using namespace qutim_sdk_0_3;

	class ManagerSettings;
	class ConnectionManager : public QObject
	{
		Q_OBJECT
		Q_CLASSINFO("DebugName", "ConnectionManager")
		Q_CLASSINFO("Service", "Bearer")
		Q_CLASSINFO("Uses", "SettingsLayer")
	public:
		ConnectionManager();
		virtual ~ConnectionManager();
	protected slots:
		void onOnlineStateChanged( bool isOnline );
		void onAccountCreated(qutim_sdk_0_3::Account *account); //autoconnect on startup
		void onStatusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status);
		void onStatusChangeTimeout();
	private:
		QTimer *getTimer(Account *account, bool create = true);
		void removeTimer(QTimer *timer);
#ifdef HAVE_NETWORK_MANAGER
		QPointer<QNetworkConfigurationManager> m_network_conf_manager;
#endif
		GeneralSettingsItem<ManagerSettings> *m_item;
		QMap<Account*, QTimer*> m_timers;
	};

}
#endif // CONNECTIONMANAGER_H

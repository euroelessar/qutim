#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <qutim/plugin.h>
#include <qutim/settingslayer.h>

namespace qutim_sdk_0_3
{
	class Account;
}

class QNetworkConfigurationManager;
namespace ConnectionManager
{
	using namespace qutim_sdk_0_3;

	class ManagerSettings;
	class ConnectionManager : public Plugin
	{
		Q_OBJECT
		Q_CLASSINFO("DebugName", "ConnectionManager")
	public:
		virtual ~ConnectionManager();
		virtual void init();
		virtual bool load();
		virtual bool unload();
	protected slots:
		void onOnlineStateChanged( bool isOnline );
		void onAccountCreated(qutim_sdk_0_3::Account *account); //autoconnect on startup
	private:
		QPointer<QNetworkConfigurationManager> m_network_conf_manager;
		GeneralSettingsItem<ManagerSettings> *m_item;
	};

}
#endif // CONNECTIONMANAGER_H

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <qutim/plugin.h>

class QNetworkConfigurationManager;
namespace ConnectionManager
{
	using namespace qutim_sdk_0_3;

	class ConnectionManager : public Plugin
	{
		Q_OBJECT
		Q_CLASSINFO("DebugName", "ConnectionManager")
	public:
		ConnectionManager();
		virtual ~ConnectionManager();
		virtual void init();
		virtual bool load();
		virtual bool unload();
	protected slots:
		void onOnlineStateChanged( bool isOnline );
	private:
		QScopedPointer<QNetworkConfigurationManager> m_network_conf_manager;
	};

}
#endif // CONNECTIONMANAGER_H

#include "../../include/player.h"
#include <QtDBus>

namespace qutim_sdk_0_3 {

namespace nowplaying {
	
	class MprisPlayerFactory : public QObject, public PlayerFactory
	{
		Q_OBJECT
		Q_INTERFACES(qutim_sdk_0_3::nowplaying::PlayerFactory)
	public:
		MprisPlayerFactory();
		QMap<QString, Player::Info> players();
		QObject *player(const QString &id);
	private slots:
		void onNameOwnerChanged(const QString &service, const QString &oldName, const QString &newName);
		void onIdentityReceived(QDBusPendingCallWatcher *watcher);
		void onDesktopNameReceived(QDBusPendingCallWatcher *watcher);
		void onNamesReceived(QDBusPendingCallWatcher *watcher);
	private:
		enum InfoType { Name, DesktopId };
		void ensureServiceInfo(const QString &service, InfoType type);
		QMap<QString, Player::Info> m_knownPlayers;
	};
} }

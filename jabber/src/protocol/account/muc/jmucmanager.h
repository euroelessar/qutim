#ifndef JMUCMANAGER_H
#define JMUCMANAGER_H

#include <QObject>
#include <qutim/chatunit.h>

namespace Jabber
{
	using namespace qutim_sdk_0_3;

	class JAccount;
	class JBookmarkManager;
	struct JMUCManagerPrivate;

	class JMUCManager : public QObject
	{
		Q_OBJECT
		public:
			JMUCManager(JAccount *account, QObject *parent = 0);
			~JMUCManager();
			ChatUnit *muc(const QString &jid);
			JBookmarkManager *bookmarkManager();
			void openJoinWindow(const QString &conference, const QString &nick, const QString &password);
			void openJoinWindow();
			void syncBookmarks();
			void join(const QString &conference, const QString &nick, const QString &password);
		private:
			QScopedPointer<JMUCManagerPrivate> p;
	};
}

#endif // JMUCMANAGER_H

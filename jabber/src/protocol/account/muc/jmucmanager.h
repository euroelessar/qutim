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
	class JMUCSession;

	class JMUCManager : public QObject
	{
		Q_OBJECT
		public:
			JMUCManager(JAccount *account, QObject *parent = 0);
			~JMUCManager();
			ChatUnit *muc(const QString &jid);
			JBookmarkManager *bookmarkManager();
			void openJoinWindow(const QString &conference, const QString &nick, const QString &password,
					const QString &name = "");
			void openJoinWindow();
			void syncBookmarks();
			void join(const QString &conference, const QString &nick = QString(), const QString &password = QString());
			void leave(const QString &room);
			bool event(QEvent *event);
		private slots:
			void bookmarksChanged();
			void join();
			void leave();
			void saveToBookmarks();
			void removeFromBookmarks();
			void copyJIDToClipboard();
		protected:
			void createActions(JMUCSession *room);
		private:
			QScopedPointer<JMUCManagerPrivate> p;
	};
}

#endif // JMUCMANAGER_H

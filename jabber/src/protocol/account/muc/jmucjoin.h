#ifndef JMUCJOIN_H
#define JMUCJOIN_H

#include <QDialog>

namespace Jabber
{
	class JAccount;
	struct JMUCJoinPrivate;

	class JMUCJoin : public QDialog {
		Q_OBJECT
		Q_DECLARE_PRIVATE(JMUCJoin)
		public:
			JMUCJoin(JAccount *account, QWidget *parent = 0);
			~JMUCJoin();
			void setConference(const QString &conference, const QString &nick, const QString &password,
					const QString &name = "");
		protected:
			void setInterface();
			void setEditConference(const QString &name, const QString &conference,
					const QString &nick, const QString &password, bool autojoin = false);
			void setEnterConference(const QString &conference, const QString &nick, const QString &password);
		private slots:
			void fillBookmarks();
			void switchScene(bool);
			void searchConference(const QString &);
			void on_buttonJoin_clicked();
			void on_buttonSearch_clicked();
			void on_buttonSave_clicked();
			void on_buttonDelete_clicked();
			void editBookmarkChanged(int);
			void enterBookmarkChanged(int);
		private:
			QScopedPointer<JMUCJoinPrivate> d_ptr;
	};
}

#endif // JMUCJOIN_H

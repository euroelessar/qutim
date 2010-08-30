#ifndef JMOODCHOOSER_H
#define JMOODCHOOSER_H

#include <QDialog>
#include "jpersonmoodconverter.h"

class QListWidgetItem;
namespace qutim_sdk_0_3 {
	class Account;
}

namespace Jabber {

	using qutim_sdk_0_3::Account;
	namespace Ui {
		class JMoodChooserWindow;
	}

	class JMoodChooserWindow : public QDialog
	{
		Q_OBJECT
	public:
		explicit JMoodChooserWindow(Account *account, const QString &text,
									const QString &mood, QWidget *parent = 0);
		~JMoodChooserWindow();
	private slots:
		void sendMood();
		void onCurrentItemChanged(QListWidgetItem *item);
	private:
		Ui::JMoodChooserWindow *ui;
		Account *m_account;
		QListWidgetItem *m_noMoodItem;
	};

	class JMoodChooser : public QObject, public JabberExtension
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JabberExtension)
	public:
		JMoodChooser();
		virtual void init(qutim_sdk_0_3::Account *account, const JabberParams &params);
	private slots:
		void showMoodChooser(QObject *obj);
	protected:
		bool eventFilter(QObject *obj, QEvent *event);
	private:
		int m_eventId;
		Account *m_account;
		QString m_currentMood;
		QString m_currentText;
	};

} // namespace Jabber
#endif // JMOODCHOOSER_H

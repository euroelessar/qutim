#ifndef JACTIVITYCHOOSER_H
#define JACTIVITYCHOOSER_H

#include <QDialog>
#include "jpersonactivityconverter.h"
#include <qutim/actiongenerator.h>

class QTreeWidgetItem;
namespace qutim_sdk_0_3 {
	class Account;
}

namespace Jabber {

	using qutim_sdk_0_3::Account;
	namespace Ui {
		class JActivityChooserWindow;
	}

	class JActivityChooserWindow : public QDialog
	{
		Q_OBJECT
	public:
		explicit JActivityChooserWindow(Account *account,
										const QString &currentText,
										const QString &currentGeneral,
										const QString &currentSpecific,
										QWidget *parent = 0);
		~JActivityChooserWindow();
	private slots:
		void sendMood();
		void onCurrentItemChanged(QTreeWidgetItem *item);
	private:
		Ui::JActivityChooserWindow *ui;
		QTreeWidgetItem *m_noActivityItem;
		Account *m_account;
	};

	class JActivityChooser : public QObject, public JabberExtension
	{
		Q_OBJECT
		Q_INTERFACES(Jabber::JabberExtension)
	public:
		JActivityChooser();
		virtual void init(qutim_sdk_0_3::Account *account, const JabberParams &params);
	private slots:
		void show(QObject *obj);
	protected:
		bool eventFilter(QObject *obj, QEvent *event);
	private:
		int m_eventId;
		Account *m_account;
		QString m_currentGeneral;
		QString m_currentSpecific;
		QString m_currentText;
	};

	class JActivityChooserAction : public qutim_sdk_0_3::ActionGenerator
	{
	public:
		JActivityChooserAction(const QIcon &icon, const qutim_sdk_0_3::LocalizedString &text,
						   const QObject *receiver, const char *member);
		JActivityChooserAction(const QIcon &icon, const qutim_sdk_0_3::LocalizedString &text,
						   const char *member);
	protected:
		virtual void showImpl(QAction *action, QObject *obj);
	};

} // namespace Jabber
#endif // JACTIVITYCHOOSER_H

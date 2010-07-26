#ifndef JOINGROUPCHAT_H
#define JOINGROUPCHAT_H

#include <QDialog>
#include <libqutim/actiongenerator.h>

class QHBoxLayout;

namespace Ui {
    class JoinGroupChat;
}

namespace Core
{
	using namespace qutim_sdk_0_3;
	
	class JoinGroupChat : public QDialog
	{
		Q_OBJECT		
	public:
		explicit JoinGroupChat(QWidget *parent = 0);
		~JoinGroupChat();
		virtual void showEvent(QShowEvent* );
	protected:
		virtual void changeEvent(QEvent* );
	private slots:
		void onToolBarActTriggered(QAction*);
		void onPositiveActTriggered();
		void onNegativeActTriggered();
		void onCurrentChanged(int);
		void onAccountBoxActivated(int index);
		void fillBookmarks();
		void fillAccounts();
	private:
		void fillBookmarks(const QVariantList &items, bool recent = false);
		Ui::JoinGroupChat *ui;
		QAction *m_positive_softkey;
		QAction *m_negative_softkey;
		QPointer<QWidget> m_dataform_widget;
	};

}
#endif // JOINGROUPCHAT_H

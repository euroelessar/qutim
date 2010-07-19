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
		void fillBookmarks();
		void fillAccounts();
	private:
		Ui::JoinGroupChat *ui;
#if defined (Q_OS_SYMBIAN)
		//in S60 just use softkey actions
		QAction *m_positive_softkey;
		QAction *m_negative_softkey;
#else
		QPushButton *m_positive_button;
		QPushButton *m_negative_button;
#endif
	};

}
#endif // JOINGROUPCHAT_H

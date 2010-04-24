#ifndef S60MAINWINDOW_H
#define S60MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class S60MainWindow;
}


namespace Core
{
	namespace Mobile
	{

		class S60MainWindow :  public QMainWindow {
			Q_OBJECT
			Q_CLASSINFO("Service", "MainWindow")
			Q_CLASSINFO("Uses", "ContactList")
		public:
			S60MainWindow(QWidget *parent = 0);
			~S60MainWindow();
		protected:
			void changeEvent(QEvent *e);
		private slots:
			void tabIndexChanged(int);
		private:
			Ui::S60MainWindow *ui;
			QAction *m_action; //right action in Symbian menu
			QMenu *m_menu; //left action in Symbian menu
		};

	}
}

#endif // S60MAINWINDOW_H

#include "s60mainwindow.h"
#include "ui_s60mainwindow.h"
#include "modulemanagerimpl.h"
#include <libqutim/menucontroller.h>
#include <libqutim/icon.h>
#include <libqutim/contactlist.h>
#include <libqutim/debug.h>

namespace Core
{
	namespace Mobile
	{
		static Core::CoreModuleHelper<S60MainWindow> s60_main_window_static(
				QT_TRANSLATE_NOOP("Plugin", "S60 Main Window"),
				QT_TRANSLATE_NOOP("Plugin", "Default qutIM GUI implementation for Symbian")
				);

		S60MainWindow::S60MainWindow(QWidget *parent) :
				QMainWindow(parent),
				ui(new Ui::S60MainWindow),
				m_menu(0)
		{
			ui->setupUi(this);
			m_action = new QAction(tr("Status"),this);
			m_action->setMenu(new QMenu(this));
			m_action->setSoftKeyRole(QAction::NegativeSoftKey);
			addAction(m_action);
			showMaximized();

			ActionGenerator *gen = new ActionGenerator(Icon("application-exit"),
													   QT_TRANSLATE_NOOP("ContactList","&Quit"),
													   qApp,
													   SLOT(quit()));
			gen->setPriority(-1);
			MenuController::addAction<ContactList>(gen);

			//Let's the black magic begin, follow the movement of hands.
			if (QObject *obj = getService("ContactList")) {
				QWidget *widget;
				QMetaObject::invokeMethod(obj, "widget", Qt::DirectConnection,
										  Q_RETURN_ARG(QWidget*, widget)
										  );
				if (widget) {
					ui->tabWidget->addTab(widget,tr("Contacts"));
					m_menu = qobject_cast<MenuController*>(obj)->menu(false);
				}
			}

			ui->tabWidget->addTab(new QWidget(this),tr("Chat"));
			ui->tabWidget->addTab(new QWidget(this),tr("Activity"));
			ui->tabWidget->setCurrentIndex(0);
			
			connect(ui->tabWidget,SIGNAL(currentChanged(int)),SLOT(tabIndexChanged(int)));			
		}

		S60MainWindow::~S60MainWindow()
		{
			delete ui;
		}

		void S60MainWindow::changeEvent(QEvent *e)
		{
			QMainWindow::changeEvent(e);
			switch (e->type()) {
			case QEvent::LanguageChange:
				ui->retranslateUi(this);
				break;
			default:
				break;
			}
		}

		void S60MainWindow::tabIndexChanged(int)
		{
			QWidget *widget = ui->tabWidget->currentWidget();
			QString actions_title = widget->property("actionsTitle").toString();
			m_action->setText(actions_title.isEmpty() ? tr("Actions") : actions_title);
			m_action->menu()->clear();
			m_action->menu()->addActions(widget->actions());

			ui->menubar->clear();
			//ui->menubar->addActions(m_menu);
			debug() << m_menu;
			m_menu->popup(QCursor::pos());
			ui->menubar->addMenu(m_menu);

		}
	}

}

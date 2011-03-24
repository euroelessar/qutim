#include "oldwidget.h"
#include <QPushButton>
#include <QAction>
#include <qutim/qtwin.h>
#include <abstractcontactmodel.h>
#include <qutim/servicemanager.h>
#include <QAbstractItemDelegate>
#include <QHBoxLayout>
#include <qutim/icon.h>
#include <qutim/config.h>
#include <qutim/utils.h>
#include <qutim/actiontoolbar.h>
#include <qutim/account.h>
#include <qutim/contact.h>
#include <qutim/systemintegration.h>
#include <simplestatusdialog.h>
#include <qutim/protocol.h>
#include <qutim/shortcut.h>
#include <QApplication>
#include <QLineEdit>
#include <qutim/metacontact.h>
#include <QDesktopWidget>
#include <qutim/actiongenerator.h>
#include <QTimer>
#include <QToolButton>

namespace Core {
namespace SimpleContactList {


OldWidget::OldWidget()
{
	connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(deleteLater()));
	setWindowIcon(Icon("qutim"));

	resize(150,0);//hack
	setAttribute(Qt::WA_AlwaysShowToolTips);
	loadGeometry();

	QWidget *w = new QWidget(this);
	setCentralWidget(w);
	setUnifiedTitleAndToolBarOnMac(true);

	QVBoxLayout *layout = new QVBoxLayout(w);
	layout->setMargin(0);
	layout->setSpacing(0);

	if (QtWin::isCompositionEnabled()) {
		QtWin::extendFrameIntoClientArea(this);
		setContentsMargins(0, 0, 0, 0);
	}


	int size = 16;
	QSize toolbar_size (size,size);
	m_mainToolBar = new ActionToolBar(this);
	m_statusToolBar = new ActionToolBar(this);
	m_mainToolBar->setWindowTitle(tr("Main Toolbar"));
	addToolBar(Qt::TopToolBarArea, m_mainToolBar);
	addToolBar(Qt::BottomToolBarArea, m_statusToolBar);

	m_statusToolBar->setIconSize(toolbar_size);
	m_statusToolBar->setFloatable(false);
	m_statusToolBar->setMovable(false);
	m_statusToolBar->setMoveHookEnabled(true);
	m_statusToolBar->setObjectName(QLatin1String("statusBar"));

	m_mainToolBar->setIconSize(toolbar_size);
	m_mainToolBar->setFloatable(false);
	m_mainToolBar->setMovable(false);
	m_mainToolBar->setMoveHookEnabled(true);
	m_mainToolBar->setObjectName(QLatin1String("contactListBar"));

#ifdef Q_WS_WIN
	m_mainToolBar->setStyleSheet("QToolBar{background:none;border:none;}");
	m_statusToolBar->setStyleSheet("QToolBar{background:none;border:none;}");
#endif

	m_model = ServiceManager::getByName<AbstractContactModel*>("ContactModel");
	m_view = new TreeView(m_model, this);
	layout->addWidget(m_view);
	m_view->setItemDelegate(ServiceManager::getByName<QAbstractItemDelegate*>("ContactDelegate"));

	m_searchBar = new QLineEdit(this);

	//m_statusBtn = new QPushButton(Icon("im-user-online"),
	//							   tr("Status"),
	//							   this);
	//m_statusBtn->setMenu(statusMenu);
	//key = new Shortcut("contactListGlobalStatus", m_statusBtn);
	//connect(key,SIGNAL(activated()), m_statusBtn, SLOT(showMenu()));
	//m_statusBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

	#if (QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))
	m_searchBar->setPlaceholderText(tr("Search contact"));
	#endif

	layout->addWidget(m_searchBar);
	connect(m_searchBar, SIGNAL(textChanged(QString)), m_model, SLOT(filterList(QString)));

	foreach(Protocol *proto, Protocol::all()) {
		connect(proto, SIGNAL(accountCreated(qutim_sdk_0_3::Account*)), this, SLOT(onAccountCreated(qutim_sdk_0_3::Account*)));
		foreach(Account *account, proto->accounts()) {
			onAccountCreated(account);
		}
	}
	QTimer::singleShot(0, this, SLOT(init()));
}

OldWidget::~OldWidget()
{
	Config config;
	config.beginGroup("contactList");
	config.setValue("geometry", saveGeometry());
}

void OldWidget::addButton(ActionGenerator *generator)
{
	m_mainToolBar->addAction(generator);
}

void OldWidget::removeButton(ActionGenerator *)
{
	//m_mainToolBar->removeAction(generator);
}

void OldWidget::loadGeometry()
{
	QByteArray geom = Config().group("contactList").value("geometry", QByteArray());
	if (geom.isNull()) {
		QRect rect = QApplication::desktop()->availableGeometry(QCursor::pos());
		//black magic
		int width = size().width();
		int x = rect.width() - width;
		int y = 0;
		int height = rect.height();
#ifdef Q_WS_WIN
		//for stupid windows
		x -= 15;
		y += 35;
		height -= 55;
#endif
		QRect geometry(x,
					   y,
					   width,
					   height
					   );
		setGeometry(geometry);
	} else {
		restoreGeometry(geom);
	}
}

void OldWidget::onAccountCreated(qutim_sdk_0_3::Account *account)
{
	//TODO add account icon
	QAction *action = new QAction(account->status().icon(), account->id(), this);
	connect(account, SIGNAL(statusChanged(qutim_sdk_0_3::Status,qutim_sdk_0_3::Status)),
			this, SLOT(onAccountStatusChanged(qutim_sdk_0_3::Status)));
	connect(account, SIGNAL(destroyed(QObject*)),
			this, SLOT(onAccountDestroyed(QObject*)));
	m_actions.insert(account, action);
	action->setMenu(account->menu());

	QToolButton *btn = new QToolButton(this);
	connect(action, SIGNAL(destroyed(QObject*)), btn, SLOT(deleteLater()));
	btn->setDefaultAction(action);
	btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
	btn->setAutoRaise(true);
	btn->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	btn->setPopupMode(QToolButton::InstantPopup);
	m_statusToolBar->addWidget(btn);
}

void OldWidget::onAccountStatusChanged(const qutim_sdk_0_3::Status &status)
{
	Account *account = sender_cast<Account *>(sender());
	QAction	*action = m_actions.value(account);
	action->setIcon(status.icon());
}

void OldWidget::showStatusDialog()
{
	//QString text = m_status_action->data().toString();
	//SimpleStatusDialog *dialog = new SimpleStatusDialog(text, this);
	//connect(dialog, SIGNAL(accepted()), SLOT(changeStatusTextAccepted()));
	//centerizeWidget(dialog);
	//SystemIntegration::show(dialog);
}

void OldWidget::changeStatusTextAccepted()
{
	//SimpleStatusDialog *dialog = qobject_cast<SimpleStatusDialog *>(sender());
	//Q_ASSERT(dialog);
	//QString text = dialog->statusText();
	//m_status_action->setData(text);
	//m_statusBtn->setToolTip(text);
	//foreach(Protocol *proto, Protocol::all()) {
	//	foreach(Account *account, proto->accounts()) {
	//		Status status = account->status();
	//		status.setText(text);
	//		account->setStatus(status);
	//	}
	//}
	//Config config = Config().group("contactList");
	//config.setValue("lastStatus",text);
	//config.sync();
}

bool OldWidget::event(QEvent *event)
{
	if (event->type() == QEvent::LanguageChange) {
		//foreach (QAction *action,m_statusActions) {
		//	Status last = m_statusBtn->property("lastStatus").value<Status>();
		//	m_statusBtn->setText(last.name());
		//	Status::Type type = static_cast<Status::Type>(action->data().toInt());
		//	action->setText(Status(type).name());
		//}
		//m_status_action->setText(tr("Set Status Text"));
		//event->accept();
	}
	return QMainWindow::event(event);
}

void OldWidget::init()
{
	MenuController *controller = ServiceManager::getByName<MenuController*>("ContactList");
	ActionGenerator *gen = new MenuActionGenerator(Icon("show-menu"), QByteArray(), controller);
	gen->setShortcut(Shortcut::getSequence("contactListActivateMainMenu").key);
	QAction *before = m_mainToolBar->actions().count() ? m_mainToolBar->actions().first() : 0;
	m_mainToolBar->insertAction(before, gen);
	SystemIntegration::show(this);
}

void OldWidget::onAccountDestroyed(QObject *obj)
{
	Account *account = reinterpret_cast<Account*>(obj);
	m_actions.take(account)->deleteLater();
}

} // namespace SimpleContactList
} // namespace Core


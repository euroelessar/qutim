#include "joingroupchat.h"
#include "ui_joingroupchat.h"
#include <QEvent>
#include <libqutim/protocol.h>
#include <libqutim/account.h>
#include <libqutim/icon.h>
#include <QPushButton>

#if defined (Q_OS_SYMBIAN)
#	define positive_key m_positive_softkey
#	define negative_key m_negative_softkey
#else
#	define positive_key m_positive_button
#	define negative_key m_negative_button
#endif

namespace Core
{

	JoinGroupChat::JoinGroupChat(QWidget *parent) :
		QDialog(parent),
		ui(new Ui::JoinGroupChat)
	{
		ui->setupUi(this);
		
#if defined (Q_OS_SYMBIAN)
		m_positive_softkey = new QAction(this);
		m_positive_softkey->setSoftKeyRole(QAction::PositiveSoftKey);
		m_negative_softkey = new QAction(this);
		m_negative_softkey->setSoftKeyRole(QAction::NegativeSoftKey);
		
		connect(m_positive_softkey,SIGNAL(triggered(bool)),SLOT(onPositiveActTriggered()));
		connect(m_negative_softkey,SIGNAL(triggered(bool)),SLOT(onNegativeActTriggered()));
#else
		QHBoxLayout *hbox_layout = new QHBoxLayout(this);
		hbox_layout->setMargin(6);
		layout()->addItem(hbox_layout);
		m_positive_button = new QPushButton(this);
		m_negative_button = new QPushButton(this);
		
		hbox_layout->addWidget(m_negative_button);
		hbox_layout->addWidget(m_positive_button);
		
		connect(m_positive_button,SIGNAL(toggled(bool)),SLOT(onPositiveActTriggered()));
		connect(m_negative_button,SIGNAL(toggled(bool)),SLOT(onNegativeActTriggered()));
#endif
		
		ui->toolBar->setIconSize(QSize(32,32));
		ui->toolBar->setToolButtonStyle(Qt::ToolButtonFollowStyle);
		ui->stackedWidget->setCurrentIndex(0);
		
		QActionGroup *group = new QActionGroup(this);
		
		QAction *act = new QAction (Icon("system-users"),QT_TRANSLATE_NOOP("JoinGroupChat", "Enter"),this);
		act->setData(0);
		act->setCheckable(true);
		act->setChecked(true);
		ui->toolBar->addAction(act);
		group->addAction(act);
		
		act = new QAction (Icon("bookmarks-organize"),QT_TRANSLATE_NOOP("JoinGroupChat", "Bookmarks"),this);
		act->setData(1);
		act->setCheckable(true);
		ui->toolBar->addAction(act);
		group->addAction(act);
		
		group->setExclusive(true);
		connect(group,SIGNAL(triggered(QAction*)),SLOT(onToolBarActTriggered(QAction*)));
		connect(ui->stackedWidget,SIGNAL(currentChanged(int)),SLOT(onCurrentChanged(int)));
		
		onCurrentChanged(0);
	}

	JoinGroupChat::~JoinGroupChat()
	{
		delete ui;
	}
	
	void JoinGroupChat::changeEvent(QEvent *e)
	{
		QDialog::changeEvent(e);
		switch (e->type()) {
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
		}
	}
	
	void JoinGroupChat::showEvent(QShowEvent *ev)
	{
		QDialog::showEvent(ev);
		fillAccounts();
		fillBookmarks();
	}

	void JoinGroupChat::onToolBarActTriggered(QAction *act)
	{
		int index = act->data().toInt();
		ui->stackedWidget->setCurrentIndex(index);
	}
	
	void JoinGroupChat::onNegativeActTriggered()
	{

	}

	void JoinGroupChat::onPositiveActTriggered()
	{

	}
	
	void JoinGroupChat::onCurrentChanged( int index)
	{
		if (!index) {
			positive_key->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Join"));
			negative_key->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Search"));
		}
		else {
			positive_key->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Save"));
			negative_key->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Delete"));
		}
	}
	
	void JoinGroupChat::fillBookmarks()
	{
		ui->bookmarksBox->clear();
		ui->bookmarksBox->addItem(QT_TRANSLATE_NOOP("JoinGroupChat", ""));
	}

	void JoinGroupChat::fillAccounts()
	{
		ui->accountBox->clear();
		foreach (Protocol *p,allProtocols()) {
			bool support = 	p->data(qutim_sdk_0_3::Protocol::ProtocolSupportGroupChat).toBool();
			if (support) {
				foreach (Account *a,p->accounts()) {
					ui->accountBox->addItem(a->id(),qVariantFromValue(a));
				}
			}
		}
	}


}

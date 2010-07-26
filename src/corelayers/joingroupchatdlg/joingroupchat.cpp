#include "joingroupchat.h"
#include "ui_joingroupchat.h"
#include <QEvent>
#include <libqutim/protocol.h>
#include <libqutim/account.h>
#include <libqutim/icon.h>
#include <libqutim/event.h>
#include <libqutim/dataforms.h>
#include <QPushButton>

namespace Core
{

	JoinGroupChat::JoinGroupChat(QWidget *parent) :
		QDialog(parent),
		ui(new Ui::JoinGroupChat)
	{
		ui->setupUi(this);
		
		m_positive_softkey = new QAction(this);
		m_positive_softkey->setSoftKeyRole(QAction::PositiveSoftKey);
		m_negative_softkey = new QAction(this);
		m_negative_softkey->setSoftKeyRole(QAction::NegativeSoftKey);

		ui->actionBox->addAction(m_positive_softkey);
		ui->actionBox->addAction(m_negative_softkey);
		
		connect(m_positive_softkey,SIGNAL(triggered(bool)),SLOT(onPositiveActTriggered()));
		connect(m_negative_softkey,SIGNAL(triggered(bool)),SLOT(onNegativeActTriggered()));

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
		connect(ui->accountBox,SIGNAL(activated(int)),SLOT(onAccountBoxActivated(int)));
		
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
		if (!ui->stackedWidget->currentIndex()) {
			if (m_dataform_widget) {
				Event event("groupchat-join",qVariantFromValue(m_dataform_widget.data()));
				int index = ui->accountBox->currentIndex();
				Account *account = ui->accountBox->itemData(index).value<Account*>();
				Q_ASSERT(account);
				qApp->sendEvent(account,&event);
			}
		}
	}
	
	void JoinGroupChat::onCurrentChanged( int index)
	{
		if (!index) {
			m_positive_softkey->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Join"));
			m_negative_softkey->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Search"));
		}
		else {
			m_positive_softkey->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Save"));
			m_negative_softkey->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Delete"));
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
					if (a->status() != Status::Offline)
						ui->accountBox->addItem(a->id(),qVariantFromValue(a));
				}
			}
		}
		if (ui->accountBox->count())
			onAccountBoxActivated(0);
	}

	void JoinGroupChat::onAccountBoxActivated(int index)
	{
		Account *account = ui->accountBox->itemData(index).value<Account*>();
		Q_ASSERT(account);
		Event event("groupchat-fields");
		qApp->sendEvent(account,&event);
		DataItem items = event.at<DataItem>(0);

		if (m_dataform_widget)
			m_dataform_widget->deleteLater();

		m_dataform_widget = AbstractDataForm::get(items);
		if (m_dataform_widget) {
			m_dataform_widget->setParent(this);
			m_dataform_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
			ui->joinPageLayout->insertWidget(0, m_dataform_widget.data());
		}

		ui->bookmarksBox->clear();
		event = Event("groupchat-bookmarks");
		qApp->sendEvent(account,&event);
		QVariantList bookmarks = event.at<QVariantList>(0);
		foreach (const QVariant &data,bookmarks) {
			QVariantMap item = data.toMap();
			QString name = item.value("name").toString();
			ui->bookmarksBox->addItem(name,item.value("fields"));
		}
	}


}

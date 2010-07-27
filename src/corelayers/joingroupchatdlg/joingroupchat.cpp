#include "joingroupchat.h"
#include "ui_joingroupchat.h"
#include <QEvent>
#include <libqutim/protocol.h>
#include <libqutim/account.h>
#include <libqutim/icon.h>
#include <libqutim/event.h>
#include <libqutim/dataforms.h>
#include <QPushButton>
#include <libqutim/debug.h>
#include <QCommandLinkButton>
#include <QLatin1Literal>

namespace Core
{

	JoinGroupChat::JoinGroupChat(QWidget *parent) :
			QDialog(parent),
			ui(new Ui::JoinGroupChat)
	{
		ui->setupUi(this);

		setAttribute(Qt::WA_DeleteOnClose);
		ui->stackedWidget->setCurrentIndex(0);
		
		connect(ui->stackedWidget,SIGNAL(currentChanged(int)),SLOT(onCurrentChanged(int)));
		connect(ui->accountBox,SIGNAL(currentIndexChanged(int)),SLOT(onAccountBoxActivated(int)));
		connect(ui->bookmarksBox,SIGNAL(currentIndexChanged(int)),SLOT(onBookmarksBoxActivated(int)));
		
		onCurrentChanged(0);
#ifdef QUTIM_MOBILE_UI
		QAction *close_action = new QAction(QT_TRANSLATE_NOOP("JoinGroupChat", "Close"),this);
		close_action->setSoftKeyRole(QAction::NegativeSoftKey);
		ui->actionBox->addAction(close_action);
		connect(close_action,SIGNAL(triggered()),SLOT(onCloseRequested()));
#endif
		m_action = new QAction(this);
		m_action->setSoftKeyRole(QAction::PositiveSoftKey);
		connect(m_action,SIGNAL(triggered()),SLOT(onActionTriggered()));
		ui->actionBox->addAction(m_action);
		ui->actionBox->setVisible(m_action,false);
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
	}

	void JoinGroupChat::onToolBarActTriggered(QAction *act)
	{
		int index = act->data().toInt();
		ui->stackedWidget->setCurrentIndex(index);
	}

	void JoinGroupChat::onCurrentChanged( int index)
	{
		Account *account = currentAccount();
		if (!account)
			return;
		//1 - join page
		//2 - bookmarks page
		//TODO rewrite on enums
		if (index == 1) {
			Event event("groupchat-fields");
			qApp->sendEvent(account,&event);
			DataItem item = event.at<DataItem>(0);
			updateDataForm(item);
			m_action->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Join"));
			ui->actionBox->setVisible(m_action,true);
		} else if (index == 2) {
			onBookmarksBoxActivated(ui->bookmarksBox->currentIndex());
			ui->actionBox->setVisible(m_action,true);
		}
	}

	void JoinGroupChat::fillAccounts()
	{
		ui->accountBox->clear();
		foreach (Protocol *p,allProtocols()) {
			bool support = p->data(qutim_sdk_0_3::Protocol::ProtocolSupportGroupChat).toBool();
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

	void JoinGroupChat::fillBookmarks(const QVariantList &items, bool recent)
	{
		QString txt = recent ? QT_TRANSLATE_NOOP("JoinGroupChat", "Recent") : QT_TRANSLATE_NOOP("JoinGroupChat", "Bookmarks");
		QListWidgetItem *list_item = new QListWidgetItem(txt,ui->listWidget);
		ui->listWidget->addItem(list_item);
		list_item->setData(Qt::UserRole,ButtonTypeSeparator);
		if (recent)
			ui->bookmarksBox->insertSeparator(ui->bookmarksBox->count());
		foreach (const QVariant &data,items) {
			QVariantMap item = data.toMap();
			QString name = item.value("name").toString();
			QVariantMap fields = item.value("fields").toMap();
			QString description;
			QVariantMap::const_iterator it;
			for (it = fields.constBegin();it!=fields.constEnd();it++) {
				description += it.key() % QLatin1Literal(": ") % it.value().toString() % QLatin1Literal(" \n");
			}
			description.remove(description.length()-2,2); //remove last \n
			list_item = createItem(name,description);
			list_item->setData(Qt::UserRole,ButtonTypeBookmark);

			ui->bookmarksBox->addItem(name,fields);
			ui->bookmarksBox->setItemData(ui->bookmarksBox->count()-1,!recent,Qt::UserRole+1);
		}
	}

	void JoinGroupChat::onAccountBoxActivated(int index)
	{
		Account *account = ui->accountBox->itemData(index).value<Account*>();
		if (!account)
			return;
		ui->listWidget->clear();

		QListWidgetItem *item = createItem(QT_TRANSLATE_NOOP("JoinGroupChat", "New groupchat"));
		item->setData(Qt::UserRole,ButtonTypeNew);
		item = createItem(QT_TRANSLATE_NOOP("JoinGroupChat", "Manage bookmarks"),QT_TRANSLATE_NOOP("JoinGroupChat", ""));
		item->setData(Qt::UserRole,ButtonTypeEditBookmarks);
		fillBookmarks(account);
	}

	void JoinGroupChat::fillBookmarks(Account *account)
	{
		ui->bookmarksBox->clear();
		Event event ("groupchat-bookmark-list");
		qApp->sendEvent(account,&event);

		ui->bookmarksBox->addItem(QString());
		//Bookmarks
		QVariantList bookmarks = event.at<QVariantList>(0);
		fillBookmarks(bookmarks);
		//Recent items
		bookmarks = event.at<QVariantList>(1);
		fillBookmarks(bookmarks,true);
	}

	void JoinGroupChat::updateDataForm(const DataItem &items, int pos)
	{
		if (m_dataform_widget)
			m_dataform_widget->deleteLater();

		m_dataform_widget = AbstractDataForm::get(items);
		if (m_dataform_widget) {
			m_dataform_widget->setParent(this);
			m_dataform_widget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
			QVBoxLayout *layout = static_cast<QVBoxLayout*>(ui->stackedWidget->currentWidget()->layout());
			layout->insertWidget(pos,m_dataform_widget.data());
		}
	}

	QListWidgetItem *JoinGroupChat::createItem(const QString &name, const QString &description)
	{
		QListWidgetItem *item = new QListWidgetItem (ui->listWidget);
		item->setFlags(Qt::NoItemFlags);
		QCommandLinkButton *button = new QCommandLinkButton(name,description,ui->listWidget);
		ui->listWidget->setItemWidget(item,button);
		item->setSizeHint(button->size());

		m_items.insert(button,item);
		connect(button,SIGNAL(clicked()),SLOT(onCommandButtonClicked()));
		connect(button,SIGNAL(destroyed(QObject*)),SLOT(onCommandButtonDestroyed(QObject*)));
		return item;
	}

	void JoinGroupChat::onCommandButtonClicked()
	{
		Account *account = currentAccount();
		if (!account)
			return;
		QCommandLinkButton *button = qobject_cast<QCommandLinkButton*>(sender());
		QListWidgetItem *item = m_items.value(button);
		ButtonTypes type = static_cast<ButtonTypes>(item->data(Qt::UserRole).toInt());
		switch (type) {
		case ButtonTypeBookmark: {
				//fill data
				Event event("groupchat-fields");
				event.args[1] = button->text();
				event.args[2] = false;
				qApp->sendEvent(account,&event);
				DataItem item = event.at<DataItem>(0);
				//join
				event = Event("groupchat-join",qVariantFromValue(item));
				qApp->sendEvent(account,&event);
				close();
				break;
			}
		case ButtonTypeNew: {
				ui->stackedWidget->setCurrentIndex(1);
				break;
			}
		case ButtonTypeEditBookmarks: {
				ui->stackedWidget->setCurrentIndex(2);
				break;
			}
		}
	}


	void JoinGroupChat::onCommandButtonDestroyed(QObject *obj)
	{
		m_items.remove(obj);
	}

	Account *JoinGroupChat::currentAccount()
	{
		int index = ui->accountBox->currentIndex();
		return (ui->accountBox->itemData(index)).value<Account*>();
	}

	void JoinGroupChat::onCloseRequested()
	{
		close();
	}

	void JoinGroupChat::closeEvent(QCloseEvent *)
	{
		if (!ui->bookmarksBox->currentText().isEmpty())
			updateBookmark(false);
	}

	void JoinGroupChat::onActionTriggered()
	{
		Account *account = currentAccount();
		if (!account)
			return;
		if (ui->stackedWidget->currentIndex() == 1) {
			//join
			DataItem item = qobject_cast<AbstractDataForm*>(m_dataform_widget)->item();
			Event event ("groupchat-join",qVariantFromValue(item));
			if (ui->saveBookmark->isChecked()) {
				event.args[1] = ui->saveBookmark->isChecked();
				event.args[2] = ui->bookmarkName->text();
			}
			qApp->sendEvent(account,&event);
			close();
		} else if (ui->stackedWidget->currentIndex() == 2) {
			int index = ui->bookmarksBox->currentIndex();
			updateBookmark(ui->bookmarksBox->itemData(index,Qt::UserRole+1).toBool());
			fillBookmarks(account);
		}
	}

	void JoinGroupChat::updateBookmark(bool remove)
	{
		Account *account = currentAccount();
		if (!account)
			return;
		if (!remove) {
			DataItem item = qobject_cast<AbstractDataForm*>(m_dataform_widget)->item();
			Event event ("groupchat-bookmark-save",qVariantFromValue(item));
			event.args[1] = ui->bookmarksBox->currentText(); //old name
			qApp->sendEvent(account,&event);
			onCurrentChanged(2);
		} else {
			Event event ("groupchat-bookmark-remove",ui->bookmarksBox->currentText());
			qApp->sendEvent(account,&event);
			onCurrentChanged(2);
		}
	}

	void JoinGroupChat::onBookmarksBoxActivated(int index)
	{
		Account *account = currentAccount();
		if (!account || ui->stackedWidget->currentIndex() != 2)
			return;

		if (!ui->bookmarksBox->itemData(index,Qt::UserRole+1).toBool())
			m_action->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Add"));
		else
			m_action->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Remove"));

		Event event("groupchat-fields");
		event.args[1] = ui->bookmarksBox->itemText(index);
		event.args[2] = true;
		qApp->sendEvent(account,&event);
		DataItem item = event.at<DataItem>(0);
		updateDataForm(item,2);
	}

}

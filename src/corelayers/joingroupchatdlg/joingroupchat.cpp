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
#include <libqutim/icon.h>
#include "bookmarksitemdelegate.h"

namespace Core
{
	const static int m_max_recent_count = 4;

	JoinGroupChat::JoinGroupChat(QWidget *parent) :
			QDialog(parent),
			ui(new Ui::JoinGroupChat)
	{
		ui->setupUi(this);
		ui->listWidget->setItemDelegate(new BookmarksItemDelegate(this));
		ui->bookmarksBox->setItemDelegate(ui->listWidget->itemDelegate());

		setAttribute(Qt::WA_DeleteOnClose);

		connect(ui->stackedWidget,SIGNAL(currentChanged(int)),SLOT(onCurrentChanged(int)));
		connect(ui->accountBox,SIGNAL(currentIndexChanged(int)),SLOT(onAccountBoxActivated(int)));
		connect(ui->bookmarksBox,SIGNAL(currentIndexChanged(int)),SLOT(onBookmarksBoxActivated(int)));
		connect(ui->listWidget,SIGNAL(activated(QModelIndex)),SLOT(onItemActivated(QModelIndex)));

		m_negative_action = new QAction(QT_TRANSLATE_NOOP("JoinGroupChat", "Close"),this);
		m_negative_action->setSoftKeyRole(QAction::NegativeSoftKey);
		ui->actionBox->addAction(m_negative_action);
		connect(m_negative_action,SIGNAL(triggered()),SLOT(onNegativeActionTriggered()));

		m_positive_action = new QAction(this);
		m_positive_action->setSoftKeyRole(QAction::PositiveSoftKey);
		connect(m_positive_action,SIGNAL(triggered()),SLOT(onPositiveActionTriggered()));
		ui->actionBox->addAction(m_positive_action);
		m_positive_action->setVisible(false);
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
		onCurrentChanged(0);
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
		if (!index) {
			m_positive_action->setVisible(false);
			m_negative_action->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Close"));
#ifndef QUTIM_MOBILE_UI
			m_negative_action->setVisible(false);
#endif
		}
		else {
			m_negative_action->setVisible(true);
			m_positive_action->setVisible(true);
			m_negative_action->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Back"));
			if (index == 1) {
				Event event("groupchat-fields");
				qApp->sendEvent(account,&event);
				DataItem item = event.at<DataItem>(0);
				updateDataForm(item);
				m_positive_action->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Join"));
			} else if (index == 2) {
				onBookmarksBoxActivated(ui->bookmarksBox->currentIndex());
			}
		}
	}

	void JoinGroupChat::fillAccounts()
	{
		ui->accountBox->clear();
		foreach (Protocol *p,allProtocols()) {
			bool support = p->data(qutim_sdk_0_3::Protocol::ProtocolSupportGroupChat).toBool();
			if (support) {
				foreach (Account *a,p->accounts()) {
					if (a->status() != Status::Offline) {
						ui->accountBox->addItem(a->status().icon(),a->id(),qVariantFromValue(a));
					}
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
		QVariantList::const_iterator it;
		for (it=items.constBegin();it!=items.constEnd();it++) {
			QVariantMap item = it->toMap();
			QString name = item.value("name").toString();
			QVariantMap fields = item.value("fields").toMap();
			list_item = createItem(name,fields);
			list_item->setData(Qt::UserRole,ButtonTypeBookmark);
			list_item->setIcon(Icon("bookmarks"));

			int index = ui->bookmarksBox->count();
			ui->bookmarksBox->addItem(Icon("bookmarks"),name,fields);
			ui->bookmarksBox->setItemData(index,!recent,Qt::UserRole+1);
			ui->bookmarksBox->setItemData(index,fields,Qt::UserRole+2);
			if (recent && ((it - items.constBegin()) >= m_max_recent_count))
				return;
		}
	}

	void JoinGroupChat::onAccountBoxActivated(int index)
	{
		Account *account = ui->accountBox->itemData(index).value<Account*>();
		if (!account)
			return;
		ui->listWidget->clear();

		QListWidgetItem *item = createItem(QT_TRANSLATE_NOOP("JoinGroupChat", "Join"),
										   qVariantFromValue(QT_TRANSLATE_NOOP("JoinGroupChat", "Join to a new groupchat")
															 ));
		item->setData(Qt::UserRole,ButtonTypeNew);
		item->setIcon(Icon("meeting-attending"));
		item = createItem(QT_TRANSLATE_NOOP("JoinGroupChat", "Manage bookmarks"),
						  qVariantFromValue(QT_TRANSLATE_NOOP("JoinGroupChat", "Edit saved bookmarks"))
						  );
		item->setData(Qt::UserRole,ButtonTypeEditBookmarks);
		item->setIcon(Icon("bookmark-new-list"));
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

		//Nice hack
		int index = ui->bookmarksBox->count();
		ui->bookmarksBox->insertSeparator(index);
		ui->bookmarksBox->setItemData(index,qVariantFromValue(QT_TRANSLATE_NOOP("JoinGroupChat", "Recent")),Qt::DisplayRole);

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

	QListWidgetItem *JoinGroupChat::createItem(const QString &name, const QVariant &data)
	{
		//		QString description;
		//		if (data.canConvert<QVariantMap>()) {
		//			QVariantMap fields = data.toMap();
		//			QVariantMap::const_iterator it;
		//			for (it = fields.constBegin();it!=fields.constEnd();it++) {
		//				description += it.key() % QLatin1Literal(": ") % it.value().toString() % QLatin1Literal(" \n");
		//			}
		//			description.remove(description.length()-2,2); //remove last \n
		//		} else
		//			description = data.toString();

		QListWidgetItem *item = new QListWidgetItem (name,ui->listWidget);
		item->setData(Qt::UserRole+2,data);
		//		QCommandLinkButton *button = new QCommandLinkButton(name,description,ui->listWidget);
		//		ui->listWidget->setItemWidget(item,button);
		//		item->setSizeHint(button->size());
		return item;
	}

	void JoinGroupChat::onItemActivated(const QModelIndex &index)
	{
		Account *account = currentAccount();
		if (!account)
			return;		
		ButtonTypes type = static_cast<ButtonTypes>(index.data(Qt::UserRole).toInt());
		switch (type) {
		case ButtonTypeBookmark: {
				//fill data
				Event event("groupchat-fields");
				event.args[1] = index.data().toString();
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

	Account *JoinGroupChat::currentAccount()
	{
		int index = ui->accountBox->currentIndex();
		return (ui->accountBox->itemData(index)).value<Account*>();
	}

	void JoinGroupChat::onNegativeActionTriggered()
	{
		if (!ui->stackedWidget->currentIndex())
			close();
		else
			ui->stackedWidget->setCurrentIndex(0);
	}

	void JoinGroupChat::closeEvent(QCloseEvent *)
	{
		if (!ui->bookmarksBox->currentText().isEmpty())
			updateBookmark(false);
	}

	void JoinGroupChat::onPositiveActionTriggered()
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
			m_positive_action->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Add"));
		else
			m_positive_action->setText(QT_TRANSLATE_NOOP("JoinGroupChat", "Remove"));

		Event event("groupchat-fields");
		event.args[1] = ui->bookmarksBox->itemText(index);
		event.args[2] = true;
		qApp->sendEvent(account,&event);
		DataItem item = event.at<DataItem>(0);
		updateDataForm(item,2);
	}

}

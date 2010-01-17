#include <QMovie>
#include "jservicebrowser.h"
#include "jservicediscovery.h"
#include "jdiscoitem.h"
#include "../jaccount.h"
#include <qutim/debug.h>

namespace Jabber
{
	JServiceBrowserModule::JServiceBrowserModule()
	{
		m_account = 0;
	}

	void JServiceBrowserModule::init(Account *account, const JabberParams &)
	{
		debug() << Q_FUNC_INFO;
		m_account = qobject_cast<JAccount *>(account);
		account->addAction(new ActionGenerator(Icon("services"),
											   QT_TRANSLATE_NOOP("Jabber", "Service discovery"),
											   this, SLOT(showWindow())), "Additional");
	}

	void JServiceBrowserModule::showWindow()
	{
		JServiceBrowser *browser = new JServiceBrowser(m_account);
		browser->show();
	}

	struct JServiceBrowserPrivate
	{
		JAccount *account;
		QMap<int, QTreeWidgetItem *> treeItems;
		Ui::ServiceBrowser *ui;
		QMenu *contextMenu;
		bool isConference;
		int searchCount;
	};

	JServiceBrowser::JServiceBrowser(JAccount *account, bool isConference, QWidget *parent)
			: QWidget(parent), p(new JServiceBrowserPrivate)
	{
		p->account = account;
		p->isConference = isConference;
		p->searchCount = 0;
		p->ui = new Ui::ServiceBrowser();
		p->contextMenu = new QMenu();
		p->ui->setupUi(this);
		this->setWindowTitle(QApplication::translate("SearchService", "Search service",
				0, QApplication::UnicodeUTF8));
		p->ui->serviceServer->installEventFilter(this);
		p->ui->searchButton->setIcon(Icon("system-search"));
		p->ui->closeButton->setIcon(Icon("window-close"));
		p->ui->clearButton->setIcon(Icon("edit-clear-locationbar-ltr"));
		p->ui->actionSearch->setIcon(Icon("system-search"));
		//p->ui->actionJoin;
		p->ui->actionExecute->setIcon(Icon("utilities-terminal"));
		p->ui->actionSearch->setIcon(Icon("edit-find-user"));
		p->ui->actionRegister->setIcon(Icon("dialog-password"));
		p->ui->actionShowVCard->setIcon(Icon("user-identity"));
		p->ui->actionAdd->setIcon(Icon("list-add-user"));
		connect(p->ui->serviceTree, SIGNAL(itemExpanded(QTreeWidgetItem*)),
				SLOT(getItems(QTreeWidgetItem*)));
		connect(p->ui->filterLine, SIGNAL(textEdited(const QString&)),
				SLOT(filterItem(const QString&)));
		connect(p->ui->serviceTree, SIGNAL(customContextMenuRequested(QPoint)),
				SLOT(showContextMenu(QPoint)));
		searchServer(QString::fromStdString(p->account->client()->jid().server()));
		this->resize(700, 600);
		p->ui->serviceTree->setColumnWidth(0, p->ui->serviceTree->width());
		QMovie *movie = new QMovie(p->ui->labelLoader);
		movie->setFileName("loader");
		movie->start();
		p->ui->labelLoader->setMovie(movie);
		p->ui->labelLoader->setVisible(false);
	}

	JServiceBrowser::~JServiceBrowser()
	{
	}

	void JServiceBrowser::searchServer(const QString &server)
	{
		p->ui->serviceServer->addItem(server);
		on_searchButton_clicked();
	}

	void JServiceBrowser::getInfo(QTreeWidgetItem *item)
	{
		JDiscoItem *di = reinterpret_cast<JDiscoItem*>(item->data(0, Qt::UserRole+1).value<qptrdiff>());
		int id = p->account->discoManager()->getInfo(this, di);
		p->treeItems.insert(id, item);
		p->searchCount++;
		p->ui->labelLoader->setVisible(true);
	}

	void JServiceBrowser::getItems(QTreeWidgetItem *item)
	{
		JDiscoItem *di = reinterpret_cast<JDiscoItem*>(item->data(0, Qt::UserRole+1).value<qptrdiff>());
		if (!item->childCount() && (di->expand())) {
			int id = p->account->discoManager()->getItems(this, di);
			p->treeItems.insert(id, item);
		}
	}

	void JServiceBrowser::setInfo(int id)
	{
		QTreeWidgetItem *item = p->treeItems.take(id);
		if (!item)
			return;
		JDiscoItem *di = reinterpret_cast<JDiscoItem*>(item->data(0, Qt::UserRole+1).value<qptrdiff>());
		if (p->isConference && (di->hasIdentity("conference") || di->hasIdentity("server")))
				item->setHidden(false);
		if (!di->name().isEmpty())
			item->setText(0, di->name());
		else
			item->setText(0, di->jid());
		item->setIcon(0, Icon(setServiceIcon(di)));
		QString tooltip;
		tooltip = "<b>"+di->name()+"</b> ("+di->jid()+")<br/>";
		if (!di->identities().isEmpty()) {
			tooltip += tr("<br/><b>Identities:</b><br/>");
			foreach(JDiscoItem::JDiscoIdentity identity, di->identities()) {
				JDiscoItem *di = new JDiscoItem();
				di->setJID(di->jid());
				di->addIdentity(identity);
				QString img = setServiceIcon(di);
				delete di;
				tooltip += "<img src='" + img + "'> " + identity.name
						+ " (" + tr("category: ") + identity.category + ", " + tr("type: ")
						+ identity.type + ")<br/>";
			}
		}
		if (false && !di->features().isEmpty()) {
			tooltip += tr("<br/><b>Features:</b><br/>");
			foreach(QString feature, di->features())
				tooltip += feature.remove("http://jabber.org/protocol/")+"<br/>";
		}
		item->setToolTip(0, tooltip);
		if (di->expand())
			item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
		QTreeWidgetItem *parent = item->parent();
		if (parent && parent->isHidden()) {
			if (item->text(0).contains(p->ui->filterLine->text(), Qt::CaseInsensitive)) {
				QList<QTreeWidgetItem*> items;
				items << item;
				setBranchVisible(items);
			} else {
				item->setHidden(true);
			}
		}
		if (!--p->searchCount);
			p->ui->labelLoader->setVisible(false);
	}

	void JServiceBrowser::setItems(int id, const QList<JDiscoItem *> &items)
	{
		QTreeWidgetItem *parentItem = p->treeItems.take(id);
		if (!parentItem || parentItem->childCount())
			return;
		if (items.isEmpty())
			parentItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
		foreach (JDiscoItem *di, items) {
			QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
			if (p->isConference)
				item->setHidden(true);
			if (!di->name().isEmpty())
				item->setText(0, di->name());
			else
				item->setText(0, di->jid());
			item->setExpanded(false);
			item->setData(0, Qt::UserRole+1, reinterpret_cast<qptrdiff>(di));
			getInfo(item);
		}
		parentItem->setExpanded(true);
	}

	void JServiceBrowser::setError(int id)
	{
		QTreeWidgetItem *item = p->treeItems.take(id);
		if (!item)
			return;
		JDiscoItem *di = reinterpret_cast<JDiscoItem*>(item->data(0, Qt::UserRole+1).value<qptrdiff>());
		item->setDisabled(true);
		item->setToolTip(0, item->toolTip(0) + di->error());
	}

	void clean_item(QTreeWidgetItem *item)
	{
		for (int i = item->childCount(); i < 0; i--) {
			QTreeWidgetItem *child = item->child(0);
			clean_item(child);
			JDiscoItem *di = reinterpret_cast<JDiscoItem*>(item->data(0, Qt::UserRole+1).value<qptrdiff>());
			item->removeChild(child);
			delete child;
			delete di;
		}
	}

	void JServiceBrowser::on_searchButton_clicked()
	{
		clean_item(p->ui->serviceTree->invisibleRootItem());
		p->ui->serviceTree->clear();
		p->treeItems.clear();
		QTreeWidgetItem *item = new QTreeWidgetItem(p->ui->serviceTree);
		item->setText(0, p->ui->serviceServer->currentText());
		JDiscoItem *di = new JDiscoItem();
		di->setJID(p->ui->serviceServer->currentText());
		item->setData(0, Qt::UserRole+1, reinterpret_cast<qptrdiff>(di));
		getInfo(item);
	}

	void JServiceBrowser::showContextMenu(const QPoint &pos)
	{
		p->contextMenu->clear();
		QTreeWidgetItem *item = p->ui->serviceTree->itemAt(pos);
		JDiscoItem *di = reinterpret_cast<JDiscoItem*>(item->data(0, Qt::UserRole+1).value<qptrdiff>());
		foreach (JDiscoItem::JDiscoAction action, di->actions()) {
			switch (action) {
			case JDiscoItem::JDiscoJoin:
				p->contextMenu->addAction(p->ui->actionJoin);
				break;
			case JDiscoItem::JDiscoRegister:
				p->contextMenu->addAction(p->ui->actionRegister);
				break;
			case JDiscoItem::JDiscoSearch:
				p->contextMenu->addAction(p->ui->actionSearch);
				break;
			case JDiscoItem::JDiscoExecute:
				p->contextMenu->addAction(p->ui->actionExecute);
				break;
			case JDiscoItem::JDiscoAdd:
				p->contextMenu->addAction(p->ui->actionAdd);
				break;
			case JDiscoItem::JDiscoVCard:
				p->contextMenu->addAction(p->ui->actionShowVCard);
				break;
			}
		}
		if (!p->contextMenu->actions().isEmpty())
			p->contextMenu->popup(p->ui->serviceTree->mapToGlobal(pos));
	}

	void JServiceBrowser::closeEvent(QCloseEvent*)
	{
		//emit finishSearch();
	}

	bool JServiceBrowser::eventFilter(QObject *obj, QEvent *event)
	{
		if (event->type() == QEvent::KeyPress) {
			QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
			if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
				on_searchButton_clicked();
			return QObject::eventFilter(obj, event);
		} else {
			return QObject::eventFilter(obj, event);
		}
	}

	QString JServiceBrowser::setServiceIcon(JDiscoItem *di)
	{
		if (di->identities().isEmpty())
			return "";
		QString service_icon;
		if (di->hasIdentity("server")) {
			service_icon = "network-server";
		} else if (di->hasIdentity("conference", "text")) {
			if (QString::fromStdString(JID(di->jid().toStdString()).username()).isEmpty())
				service_icon = "conferenceserver";
			else if (QString::fromStdString(JID(di->jid().toStdString()).resource()).isEmpty())
				service_icon = "conference";
			else
				service_icon = "conferenceuser";
		} else if (di->hasIdentity("conference", "irc")) {
			service_icon = "irc";
		} else if (di->hasIdentity("gateway", "icq")) {
			service_icon = "icq_tr";
		} else if (di->hasIdentity("gateway", "aim")) {
			service_icon = "aim_tr";
		} else if (di->hasIdentity("gateway", "mrim")) {
			service_icon = "mrim_tr";
		} else if (di->hasIdentity("gateway", "msn")) {
			service_icon = "msn_tr";
		} else if (di->hasIdentity("gateway", "xmpp")) {
			service_icon = "jabber_tr";
		} else if (di->hasIdentity("gateway")) {
			service_icon = "default_tr";
		} else if (di->hasIdentity("directory")) {
			service_icon = "edit-find-user";
		} else if (di->hasIdentity("automation")) {
			service_icon = "utilities-terminal";
		} else {
			service_icon = "defaultservice";
		}
		return service_icon;
	}

	/*void JServiceBrowser::on_joinButton_clicked()
	{
		QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
		emit joinConference(item->text(1));
		if (p->isConference)
			close();
	}

	void JServiceBrowser::on_registerButton_clicked()
	{
		QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
		emit registerTransport(item->text(1));
	}

	void JServiceBrowser::on_searchFormButton_clicked()
	{
		QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
		JDiscoItem *di;
		di = reinterpret_cast<JDiscoItem*>(item->data(0, Qt::UserRole+1).value<qptrdiff>());
		emit searchService("", item->text(1));
	}

	void JServiceBrowser::on_executeButton_clicked()
	{
		QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
		JDiscoItem *di;
		di = reinterpret_cast<JDiscoItem*>(item->data(0, Qt::UserRole+1).value<qptrdiff>());
		emit executeCommand(item->text(1), di->node());
	}

	void JServiceBrowser::on_addRosterButton_clicked()
	{
		QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
		emit addContact(item->text(1), item->text(0));
	}

	void JServiceBrowser::on_showVCardButton_clicked()
	{
		QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
		JDiscoItem *di;
		di = reinterpret_cast<JDiscoItem*>(item->data(0, Qt::UserRole+1).value<qptrdiff>());
		emit showVCard(item->text(1));
	}

	void JServiceBrowser::on_addProxyButton_clicked()
	{
		QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
		emit addProxy(JID(utils::toStd(item->text(1))));
	}*/

	void JServiceBrowser::on_closeButton_clicked()
	{
		close();
	}

	void JServiceBrowser::filterItem(const QString &mask)
	{
		setItemVisible(p->ui->serviceTree->invisibleRootItem(), true);
		QList<QTreeWidgetItem*> visibleItems;
		visibleItems = findItems(p->ui->serviceTree->invisibleRootItem(), p->ui->filterLine->text());
		setBranchVisible(visibleItems);
	}

	void JServiceBrowser::setBranchVisible(QList<QTreeWidgetItem*> items)
	{
		int count = items.count();
		int num = 0;
		while (num < count) {
			QTreeWidgetItem *item = items[num]->parent();
			if (item && !items.contains(item)) {
				items.append(item);
				count++;
			}
			num++;
		}
		foreach(QTreeWidgetItem* item, items)
			item->setHidden(false);
	}

	QList<QTreeWidgetItem*> JServiceBrowser::findItems(QTreeWidgetItem *item, const QString &text)
	{
		QList<QTreeWidgetItem*> list;
		int count = item->childCount();
		for (int pos = 0; pos < count; pos++) {
			if (text == "" || item->child(pos)->text(0).contains(text, Qt::CaseInsensitive)) {
				list << item->child(pos);
				list << findItems(item->child(pos), "");
			} else if (item->child(pos)->childCount()) {
				list << findItems(item->child(pos), text);
			}
		}
		return list;
	}

	void JServiceBrowser::setItemVisible(QTreeWidgetItem *item, bool visibility)
	{
		item->setHidden(visibility);
		int count = item->childCount();
		for (int pos = 0; pos < count; pos++)
		setItemVisible(item->child(pos), visibility);
	}

	void JServiceBrowser::on_clearButton_clicked()
	{
		p->ui->filterLine->clear();
		filterItem("");
	}
}

#include "jservicebrowser.h"
#include "jservicediscovery.h"
#include "jdiscoitem.h"
#include "../jaccount.h"

namespace Jabber
{
	struct JServiceBrowserPrivate
	{
		JAccount *account;
		QMap<int, QTreeWidgetItem *> treeItems;
		Ui::ServiceBrowser *ui;
	};

	JServiceBrowser::JServiceBrowser(JAccount *account, QWidget *parent)
			: QDialog(parent), p(new JServiceBrowserPrivate)
	{
		p->account = account;
		//m_autoclose = autoclose;
		p->ui = new Ui::ServiceBrowser();
		p->ui->setupUi(this);
		this->setWindowTitle(QApplication::translate("SearchService", "Search service",
				0, QApplication::UnicodeUTF8));
		p->ui->serviceServer->installEventFilter(this);
		p->ui->filterButton->setCheckable(true);
		p->ui->filterLine->setVisible(false);
		connect(p->ui->serviceTree, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
				SLOT(showControls(QTreeWidgetItem*, int)));
		connect(p->ui->serviceTree, SIGNAL(itemExpanded(QTreeWidgetItem*)),
				SLOT(getChildItems(QTreeWidgetItem*)));
		connect(p->ui->filterLine, SIGNAL(textEdited(const QString&)),
				SLOT(filterItem(const QString&)));
		connect(p->ui->filterButton, SIGNAL(clicked(bool)), SLOT(showFilterLine(bool)));
		searchServer(QString::fromStdString(JID(p->account->id().toStdString()).server()));
		this->resize(700, 600);
		p->ui->serviceTree->setColumnWidth(0, p->ui->serviceTree->width()*9/10);
	}

	JServiceBrowser::~JServiceBrowser()
	{
	}

	void JServiceBrowser::searchServer(const QString &server)
	{
		p->ui->serviceServer->addItem(server);
		on_searchButton_clicked();
	}

	void JServiceBrowser::getItems(QTreeWidgetItem *item)
	{
		JDiscoItem *di = reinterpret_cast<JDiscoItem*>(item->data(0, Qt::UserRole+1).value<qptrdiff>());
		if (!item->childCount() && (di->expand())) {
			int id = p->account->discoManager()->getItems(this, di);
			p->treeItems.insert(id, item);
		}
	}

	void JServiceBrowser::getInfo(QTreeWidgetItem *item)
	{
		JDiscoItem *di = reinterpret_cast<JDiscoItem*>(item->data(0, Qt::UserRole+1).value<qptrdiff>());
		int id = p->account->discoManager()->getInfo(this, di);
		p->treeItems.insert(id, item);
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
			if (!di->name().isEmpty())
				item->setText(0, di->name());
			item->setText(1, di->jid());
			item->setExpanded(false);
			item->setData(0, Qt::UserRole+1, reinterpret_cast<qptrdiff>(di));
			getInfo(item);
		}
		parentItem->setExpanded(true);
	}

	void JServiceBrowser::setInfo(int id)
	{
		QTreeWidgetItem *item = p->treeItems.take(id);
		JDiscoItem *di = reinterpret_cast<JDiscoItem*>(item->data(0, Qt::UserRole+1).value<qptrdiff>());
		if (!item)
			return;
		item->setDisabled(false);
		if (!di->name().isEmpty())
			item->setText(0, di->name());
		item->setText(1, di->jid());
		item->setIcon(0, Icon(setServiceIcon(di)));
		QString tooltip;
		tooltip = "<b>"+di->name()+"</b> ("+di->jid()+")<br/>";
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
		if (!di->features().isEmpty()) {
			tooltip += tr("<br/><b>Features:</b><br/>");
			foreach(QString feature, di->features())
				tooltip += feature+"<br/>";
		}
		item->setToolTip(0, tooltip);
		if (di->expand())
			item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
		if (p->ui->filterButton->isChecked()) {
			QTreeWidgetItem *parent = item->parent();
			if (parent && parent->isHidden()) {
				if (item->text(0).contains(p->ui->filterLine->text(), Qt::CaseInsensitive)
						|| item->text(1).contains(p->ui->filterLine->text(), Qt::CaseInsensitive)) {
					QList<QTreeWidgetItem*> items;
					items << item;
					setBranchVisible(items);
				} else {
					item->setHidden(true);
				}
			}
		}
	}

	void JServiceBrowser::on_searchButton_clicked()
	{
		hideControls();
		foreach (int id, p->treeItems.keys()) {
			QTreeWidgetItem *item = p->treeItems.take(id);
			JDiscoItem *di = reinterpret_cast<JDiscoItem*>(item->data(0, Qt::UserRole+1).value<qptrdiff>());
			delete di;
			delete item;
		}
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, "");
		item->setText(1, p->ui->serviceServer->currentText());
		p->ui->serviceTree->addTopLevelItem(item);
		JDiscoItem *di = new JDiscoItem();
		di->setJID(p->ui->serviceServer->currentText());
		item->setData(0, Qt::UserRole+1, reinterpret_cast<qptrdiff>(di));
		getInfo(item);
	}

	void JServiceBrowser::hideControls()
	{
		p->ui->joinButton->setEnabled(false);
		p->ui->registerButton->setEnabled(false);
		p->ui->searchFormButton->setEnabled(false);
		p->ui->executeButton->setEnabled(false);
		p->ui->addRosterButton->setEnabled(false);
		p->ui->showVCardButton->setEnabled(false);
		p->ui->addProxyButton->setEnabled(false);
	}

	void JServiceBrowser::showControls(QTreeWidgetItem *item, int)
	{
		hideControls();
		JDiscoItem *di = reinterpret_cast<JDiscoItem*>(item->data(0, Qt::UserRole+1).value<qptrdiff>());
		foreach (JDiscoItem::JDiscoAction action, di->actions()) {
			switch (action) {
			case JDiscoItem::JDiscoJoin:
				p->ui->joinButton->setEnabled(true);
				break;
			case JDiscoItem::JDiscoRegister:
				p->ui->registerButton->setEnabled(true);
				break;
			case JDiscoItem::JDiscoSearch:
				p->ui->searchFormButton->setEnabled(true);
				break;
			case JDiscoItem::JDiscoExecute:
				p->ui->executeButton->setEnabled(true);
				break;
			case JDiscoItem::JDiscoAdd:
				p->ui->addRosterButton->setEnabled(true);
				break;
			case JDiscoItem::JDiscoVCard:
				p->ui->showVCardButton->setEnabled(true);
				break;
			case JDiscoItem::JDiscoProxy:
				p->ui->addProxyButton->setEnabled(true);
				break;
			}
		}
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
			service_icon = "server";
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
			service_icon = "finduser";
		} else if (di->hasIdentity("automation")) {
			service_icon = "command";
		} else {
			service_icon = "defaultservice";
		}
		return service_icon;
	}

	/*void JServiceBrowser::on_joinButton_clicked()
	{
		QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
		emit joinConference(item->text(1));
		if (m_autoclose)
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
			if (text == ""
					|| item->child(pos)->text(0).contains(text, Qt::CaseInsensitive)
					|| item->child(pos)->text(1).contains(text, Qt::CaseInsensitive)) {
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

	void JServiceBrowser::showFilterLine(bool show)
	{
		p->ui->filterLine->setText("");
		filterItem("");
		p->ui->filterLine->setVisible(show);
	}
}

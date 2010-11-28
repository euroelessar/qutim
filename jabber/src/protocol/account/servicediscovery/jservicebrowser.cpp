#include <QMovie>
#include <QStringBuilder>
#include "jservicebrowser.h"
#include "jservicediscovery.h"
#include "jdiscoitem.h"
#include "../jaccount.h"
#include "../muc/jmucmanager.h"
#include "protocol/modules/adhoc/jadhocwidget.h"
#include <qutim/iconloader.h>
#include <qutim/configbase.h>
#include <qutim/protocol.h>
#include "ui_jservicebrowser.h"
#include <qutim/debug.h>
//jreen
#include <jreen/client.h>

namespace Jabber
{
JServiceBrowserModule::JServiceBrowserModule()
{
	m_account = 0;
}

void JServiceBrowserModule::init(Account *account, const JabberParams &)
{
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
	bool showFeatures;
	JDiscoItem currentMenuItem;
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
	setWindowTitle(tr("Search service"));
	p->ui->serviceServer->installEventFilter(this);
	p->ui->serviceServer->setDuplicatesEnabled(false);
	p->ui->serviceServer->setInsertPolicy(QComboBox::NoInsert);
	connect(p->ui->serviceServer, SIGNAL(currentIndexChanged(int)), SLOT(on_searchButton_clicked()));
	p->ui->searchButton->setIcon(Icon("system-search"));
	p->ui->clearButton->setIcon(Icon("edit-clear-locationbar-rtl"));
	p->ui->actionSearch->setIcon(Icon("system-search"));
	//p->ui->actionJoin;
	p->ui->actionExecute->setIcon(Icon("utilities-terminal"));
	p->ui->actionSearch->setIcon(Icon("edit-find-user"));
	p->ui->actionRegister->setIcon(Icon("dialog-password"));
	p->ui->actionShowVCard->setIcon(Icon("user-identity"));
	p->ui->actionAdd->setIcon(Icon("list-add-user"));
	connect(p->ui->actionExecute, SIGNAL(triggered()), this, SLOT(onExecute()));
	connect(p->ui->actionJoin, SIGNAL(triggered()), this, SLOT(onJoin()));
	connect(p->ui->actionAdd, SIGNAL(triggered()), this, SLOT(onAddToRoster()));
	connect(p->ui->serviceTree, SIGNAL(itemExpanded(QTreeWidgetItem*)),
			SLOT(getItems(QTreeWidgetItem*)));
	connect(p->ui->filterLine, SIGNAL(textEdited(const QString&)),
			SLOT(filterItem(const QString&)));
	connect(p->ui->serviceTree, SIGNAL(customContextMenuRequested(QPoint)),
			SLOT(showContextMenu(QPoint)));
	connect(p->ui->serviceTree, SIGNAL(itemSelectionChanged()), SLOT(showFeatures()));
	p->ui->serviceTree->setColumnWidth(0, p->ui->serviceTree->width());
	/*QMovie *movie = new QMovie(p->ui->labelLoader);
  movie->setFileName("loader");
  movie->start();
  p->ui->labelLoader->setMovie(movie);*/
	p->ui->labelLoader->setVisible(false);
	Config group = account->protocol()->config("serviceBrowser");
	p->showFeatures = group.value("showFeatures", false);
	QStringList items = group.value("servers", QStringList());
	p->ui->serviceServer->addItems(items);
	if (!p->showFeatures)
		p->ui->splitter->setSizes(QList<int>() << 100 << 0);
	//		searchServer(QString::fromStdString(p->account->client()->jid().server()));
}

JServiceBrowser::~JServiceBrowser()
{
}

void JServiceBrowser::searchServer(const QString &server)
{
	p->ui->serviceServer->setEditText(server);
	on_searchButton_clicked();
}

void JServiceBrowser::getInfo(QTreeWidgetItem *item)
{
	JDiscoItem di = item->data(0, Qt::UserRole+1).value<JDiscoItem>();
	int id = p->account->discoManager()->getInfo(this, di);
	p->treeItems.insert(id, item);
	p->searchCount++;
	p->ui->labelLoader->setVisible(true);
}

void JServiceBrowser::getItems(QTreeWidgetItem *item)
{
	JDiscoItem di = item->data(0, Qt::UserRole+1).value<JDiscoItem>();
	if (!item->childCount() && (di.isExpandable())) {
		int id = p->account->discoManager()->getItems(this, di);
		p->treeItems.insert(id, item);
	}
}

void JServiceBrowser::setInfo(int id)
{
	QTreeWidgetItem *item = p->treeItems.take(id);
	if (!item)
		return;
	JDiscoItem di = item->data(0, Qt::UserRole+1).value<JDiscoItem>();
	if (p->isConference && (di.hasIdentity("conference") || di.hasIdentity("server")))
		item->setHidden(false);
	if (!di.name().isEmpty())
		item->setText(0, di.name());
	else
		item->setText(0, di.jid());
	item->setIcon(0, Icon(setServiceIcon(di)));
	QString tooltip;
	tooltip = QLatin1Literal("<b>") % di.name() % QLatin1Literal("</b> (")
			% di.jid() % QLatin1Literal(")<br/>");
	QString type = tr("type: ");
	QString category = tr("category: ");
	if (!di.identities().isEmpty()) {
		tooltip += QLatin1Literal("<br/><b>") % tr("Identities:") % QLatin1Literal("</b><br/>");
		foreach(JDiscoItem::Identity identity, di.identities()) {
			JDiscoItem di;
			di.setJID(di.jid());
			di.addIdentity(identity);
			QString img = setServiceIcon(di);
			tooltip += QLatin1Literal("<img src='") % img % QLatin1Literal("'> ")
					% identity.name % QLatin1Literal(" (") % category
					% identity.category % QLatin1Literal(", ") % type
					% identity.type % QLatin1Literal(")<br/>");
		}
	}
	item->setToolTip(0, tooltip);
	if (di.isExpandable())
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
	if (!--p->searchCount)
		p->ui->labelLoader->setVisible(false);
}

void JServiceBrowser::setItems(int id, const QList<JDiscoItem> &items)
{
	QTreeWidgetItem *parentItem = p->treeItems.take(id);
	if (!parentItem || parentItem->childCount())
		return;
	if (items.isEmpty())
		parentItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
	foreach (const JDiscoItem &di, items) {
		QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
		if (p->isConference)
			item->setHidden(true);
		if (!di.name().isEmpty())
			item->setText(0, di.name());
		else
			item->setText(0, di.jid());
		item->setExpanded(false);
		item->setData(0, Qt::UserRole+1, qVariantFromValue(di));
		getInfo(item);
	}
	parentItem->setExpanded(true);
}

void JServiceBrowser::setError(int id)
{
	QTreeWidgetItem *item = p->treeItems.take(id);
	if (!item)
		return;
	JDiscoItem di = item->data(0, Qt::UserRole+1).value<JDiscoItem>();
	item->setDisabled(true);
	item->setToolTip(0, item->toolTip(0) + di.error());
	if (!--p->searchCount)
		p->ui->labelLoader->setVisible(false);
}

void clean_item(QTreeWidgetItem *item)
{
	for (int i = item->childCount(); i < 0; i--) {
		QTreeWidgetItem *child = item->child(0);
		clean_item(child);
		item->removeChild(child);
		delete child;
	}
}

void JServiceBrowser::on_searchButton_clicked()
{
	disconnect(p->ui->serviceServer, SIGNAL(currentIndexChanged(int)), this, SLOT(on_searchButton_clicked()));
	p->searchCount = 0;
	clean_item(p->ui->serviceTree->invisibleRootItem());
	p->ui->serviceTree->clear();
	p->treeItems.clear();
	QString server(p->ui->serviceServer->currentText());
	QTreeWidgetItem *item = new QTreeWidgetItem(p->ui->serviceTree);
	item->setText(0, server);
	JDiscoItem di;
	di.setJID(p->ui->serviceServer->currentText());
	item->setData(0, Qt::UserRole+1, qVariantFromValue(di));
	getInfo(item);
	p->ui->serviceServer->removeItem(p->ui->serviceServer->findText(server));
	p->ui->serviceServer->insertItem(0, server);
	p->ui->serviceServer->setCurrentIndex(0);
	Config group = p->account->protocol()->config().group("serviceBrowser");
	QStringList items;
	for (int num = 0; num < p->ui->serviceServer->count(); num++)
		items << p->ui->serviceServer->itemText(num);
	group.setValue("servers", items);
	group.sync();
	connect(p->ui->serviceServer, SIGNAL(currentIndexChanged(int)), SLOT(on_searchButton_clicked()));
}

void JServiceBrowser::showContextMenu(const QPoint &pos)
{
	p->contextMenu->clear();
	QTreeWidgetItem *item = p->ui->serviceTree->itemAt(pos);
	JDiscoItem di = item->data(0, Qt::UserRole+1).value<JDiscoItem>();
	p->currentMenuItem = di;
	foreach (JDiscoItem::Action action, di.actions()) {
		switch (action) {
		case JDiscoItem::ActionJoin:
			p->contextMenu->addAction(p->ui->actionJoin);
			break;
		case JDiscoItem::ActionRegister:
			p->contextMenu->addAction(p->ui->actionRegister);
			break;
		case JDiscoItem::ActionSearch:
			p->contextMenu->addAction(p->ui->actionSearch);
			break;
		case JDiscoItem::ActionExecute:
			p->contextMenu->addAction(p->ui->actionExecute);
			break;
		case JDiscoItem::ActionAdd:
			p->contextMenu->addAction(p->ui->actionAdd);
			break;
		case JDiscoItem::ActionVCard:
			p->contextMenu->addAction(p->ui->actionShowVCard);
			break;
		case JDiscoItem::ActionProxy:
			//				p->contextMenu->addAction(p->ui->action);
			break;
		}
	}
	if (!p->contextMenu->actions().isEmpty())
		p->contextMenu->popup(p->ui->serviceTree->viewport()->mapToGlobal(pos));
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

void JServiceBrowser::showFeatures()
{
	QTreeWidgetItem *item = !p->ui->serviceTree->selectedItems().isEmpty()
			? p->ui->serviceTree->selectedItems().first() : 0;
	if (!item)
		return;
	JDiscoItem di = item->data(0, Qt::UserRole+1).value<JDiscoItem>();
	QString featuresText;
	if (!di.features().isEmpty()) {
		featuresText = QLatin1Literal("<b>") % tr("Features:") % QLatin1Literal("</b><br/>");
		QStringList features = QStringList::fromSet(di.features());
		features.sort();
		foreach(QString feature, features)
			featuresText += feature % QLatin1Literal("<br/>");
	}
	p->ui->featuresView->setHtml(featuresText);
}

QString JServiceBrowser::setServiceIcon(const JDiscoItem &di)
{
	if (di.identities().isEmpty())
		return QString();
	QString service_icon;
	if (di.hasIdentity("server")) {
		service_icon = "network-server";
	} else if (di.hasIdentity("conference", "text")) {
		if (QString::fromStdString(JID(di.jid().toStdString()).username()).isEmpty())
			service_icon = "conference-server";
		else if (QString::fromStdString(JID(di.jid().toStdString()).resource()).isEmpty())
			service_icon = "conference";
		else
			service_icon = "conference-user";
	} else if (di.hasIdentity("conference", "irc")) {
		service_icon = "im-irc-gateway";
	} else if (di.hasIdentity("gateway", "icq")) {
		service_icon = "im-icq-gateway";
	} else if (di.hasIdentity("gateway", "aim")) {
		service_icon = "im-aim-gateway";
	} else if (di.hasIdentity("gateway", "mrim")) {
		service_icon = "im-mrim-gateway";
	} else if (di.hasIdentity("gateway", "msn")) {
		service_icon = "im-msn-gateway";
	} else if (di.hasIdentity("gateway", "xmpp")) {
		service_icon = "im-jabber-gateway";
	} else if (di.hasIdentity("gateway")) {
		service_icon = "im-default-gateway";
	} else if (di.hasIdentity("directory")) {
		service_icon = "edit-find-user";
	} else if (di.hasIdentity("automation")) {
		service_icon = "utilities-terminal";
	} else {
		service_icon = "defaultservice";
	}
	return IconLoader::instance()->iconPath(service_icon, 16);
}

/*void JServiceBrowser::on_registerButton_clicked()
 {
  QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
  emit registerTransport(item->text(1));
 }

 void JServiceBrowser::on_searchFormButton_clicked()
 {
  QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
  JDiscoItem di;
  di = item->data(0, Qt::UserRole+1).value<JDiscoItem>();
  emit searchService("", item->text(1));
 }

 void JServiceBrowser::on_addRosterButton_clicked()
 {
  QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
  emit addContact(item->text(1), item->text(0));
 }

 void JServiceBrowser::on_showVCardButton_clicked()
 {
  QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
  JDiscoItem di;
  di = item->data(0, Qt::UserRole+1).value<JDiscoItem>();
  emit showVCard(item->text(1));
 }

 void JServiceBrowser::on_addProxyButton_clicked()
 {
  QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
  emit addProxy(JID(utils::toStd(item->text(1))));
 }*/

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
		if (text.isEmpty() || item->child(pos)->text(0).contains(text, Qt::CaseInsensitive)) {
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

void JServiceBrowser::onExecute()
{
	//QWidget *widget = new JAdHocWidget(p->currentMenuItem.jid().toStdString(),
	//								   p->account->connection()->adhoc());
	//centerizeWidget(widget);
	//widget->setAttribute(Qt::WA_DeleteOnClose, true);
	//widget->setAttribute(Qt::WA_QuitOnClose, false);
	//widget->show();
}

void JServiceBrowser::onJoin()
{
	if (p->isConference) {
		emit joinConference(p->currentMenuItem.jid());
		close();
	} else {
		//Rewrite on event system, something like
		//Event event("groupchat-join-request");
		//QObject *obj = getService("JoinGroupChat");
		//qApp->sendEvent(obj,&event);
		//p->account->conferenceManager()->openJoinWindow(p->currentMenuItem.jid(), p->account->name(), "");
	}
}

void JServiceBrowser::onAddToRoster()
{

}
}

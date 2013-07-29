/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2011 Ruslan Nigmatullin <euroelessar@yandex.ru>
**
*****************************************************************************
**
** $QUTIM_BEGIN_LICENSE$
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
** See the GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see http://www.gnu.org/licenses/.
** $QUTIM_END_LICENSE$
**
****************************************************************************/
#include <QMovie>
#include <QStringBuilder>
#include <QObjectCleanupHandler>
#include "jservicebrowser.h"
#include "ui_jservicebrowser.h"
// qutIM
#include <qutim/iconloader.h>
#include <qutim/config.h>
#include <qutim/protocol.h>
#include <qutim/debug.h>
#include <qutim/groupchatmanager.h>
#include <qutim/contact.h>
#include <qutim/account.h>
#include <qutim/systemintegration.h>
#include <qutim/dataforms.h>
//Jreen
#include <jreen/client.h>

namespace Jabber
{
using namespace qutim_sdk_0_3;
enum { ItemRole = Qt::UserRole + 1};

JServiceBrowserModule::JServiceBrowserModule()
{
	if (1) {} else Q_UNUSED(QT_TRANSLATE_NOOP("Menu", "Additional"));
	m_account = 0;
}

void JServiceBrowserModule::init(Account *account)
{
	if (qobject_cast<Jreen::Client*>(account->property("client"))) {
		m_account = account;
		account->addAction(new ActionGenerator(Icon("services"),
											   QT_TRANSLATE_NOOP("Jabber", "Service discovery"),
											   this, SLOT(showWindow())), "Additional");
	}
}

void JServiceBrowserModule::showWindow()
{
	JServiceBrowser *browser = new JServiceBrowser(m_account);
	SystemIntegration::show(browser);
}

struct JServiceBrowserPrivate
{
	Account *account;
	Jreen::Disco *disco;
	QObjectCleanupHandler cleanupHandler;
	Ui::ServiceBrowser *ui;
	QMenu *contextMenu;
	bool isConference;
	int searchCount;
	bool showFeatures;
	Jreen::Disco::Item currentMenuItem;
};

JServiceBrowser::JServiceBrowser(Account *account, bool isConference, QWidget *parent)
	: QWidget(parent), p(new JServiceBrowserPrivate)
{
	Jreen::Client *client = qobject_cast<Jreen::Client*>(account->property("client"));
	p->disco = client->disco();
	p->account = account;
	p->isConference = isConference; //WTF ? Oo
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

	QAction *action = new QAction(tr("Close"),this);
//	action->setSoftKeyRole(QAction::NegativeSoftKey);
	connect(action, SIGNAL(triggered()), SLOT(close()));
	addAction(action);
	setAttribute(Qt::WA_DeleteOnClose, true);
	setAttribute(Qt::WA_QuitOnClose, false);
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
	Jreen::Disco::Item di = item->data(0, ItemRole).value<Jreen::Disco::Item>();
	Jreen::DiscoReply *reply = p->disco->requestInfo(di);
	reply->setProperty("item", qVariantFromValue(item));
	p->cleanupHandler.add(reply);
	connect(reply, SIGNAL(infoReceived(Jreen::Disco::Item)),
	        SLOT(onInfoReceived(Jreen::Disco::Item)));
	connect(reply, SIGNAL(error(Jreen::Error::Ptr)),
	        SLOT(onError(Jreen::Error::Ptr)));
	p->searchCount++;
	p->ui->labelLoader->setVisible(true);
}

void JServiceBrowser::getItems(QTreeWidgetItem *item)
{
	Jreen::Disco::Item di = item->data(0, ItemRole).value<Jreen::Disco::Item>();
	if (!item->childCount() && (di.actions() & Jreen::Disco::Item::ActionExpand)) {
		Jreen::DiscoReply *reply = p->disco->requestItems(di);
		reply->setProperty("item", qVariantFromValue(item));
		p->cleanupHandler.add(reply);
		connect(reply, SIGNAL(itemsReceived(Jreen::Disco::ItemList)),
		        SLOT(onItemsReceived(Jreen::Disco::ItemList)));
		connect(reply, SIGNAL(error(Jreen::Error::Ptr)),
		        SLOT(onError(Jreen::Error::Ptr)));
	}
}

void JServiceBrowser::onInfoReceived(const Jreen::Disco::Item &di)
{
	QTreeWidgetItem *item = sender()->property("item").value<QTreeWidgetItem*>();
	Q_ASSERT(item);
	item->setData(0, ItemRole, qVariantFromValue(di));
	if (p->isConference && (di.hasIdentity("conference") || di.hasIdentity("server")))
		item->setHidden(false);
	if (!di.name().isEmpty())
		item->setText(0, di.name());
	else
		item->setText(0, di.jid());
	item->setIcon(0, Icon(serviceIcon(di)));
	QString tooltip;
	tooltip = QLatin1Literal("<b>") % di.name() % QLatin1Literal("</b> (")
			% di.jid().full() % QLatin1Literal(")<br/>");
	QString type = tr("type: ");
	QString category = tr("category: ");
	if (!di.identities().isEmpty()) {
		tooltip += QLatin1Literal("<br/><b>") % tr("Identities:") % QLatin1Literal("</b><br/>");
		foreach(Jreen::Disco::Identity identity, di.identities()) {
			Jreen::Disco::Item tmp;
			tmp.setJid(tmp.jid());
			tmp.addIdentity(identity);
			QString img = IconLoader::iconPath(serviceIcon(tmp), 16);
			tooltip += QLatin1Literal("<img src='") % img % QLatin1Literal("'> ")
					% identity.name() % QLatin1Literal(" (") % category
					% identity.category() % QLatin1Literal(", ") % type
					% identity.type() % QLatin1Literal(")<br/>");
		}
	}
	item->setToolTip(0, tooltip);
	if (di.actions() & Jreen::Disco::Item::ActionExpand)
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

void JServiceBrowser::onItemsReceived(const Jreen::Disco::ItemList &items)
{
	QTreeWidgetItem *parentItem = sender()->property("item").value<QTreeWidgetItem*>();
	Q_ASSERT(parentItem);
	if (!parentItem || parentItem->childCount())
		return;
	if (items.isEmpty())
		parentItem->setChildIndicatorPolicy(QTreeWidgetItem::DontShowIndicator);
	foreach (const Jreen::Disco::Item &di, items) {
		QTreeWidgetItem *item = new QTreeWidgetItem(parentItem);
		if (p->isConference)
			item->setHidden(true);
		if (!di.name().isEmpty())
			item->setText(0, di.name());
		else
			item->setText(0, di.jid());
		item->setExpanded(false);
		item->setData(0, ItemRole, qVariantFromValue(di));
		getInfo(item);
	}
	parentItem->setExpanded(true);
}

void JServiceBrowser::onError(const Jreen::Error::Ptr &error)
{
	QTreeWidgetItem *item = sender()->property("item").value<QTreeWidgetItem*>();
	Q_ASSERT(item);
	item->setDisabled(true);
	item->setToolTip(0, item->toolTip(0) + error->conditionText());
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
	p->cleanupHandler.clear();
	QString server(p->ui->serviceServer->currentText());
	QTreeWidgetItem *item = new QTreeWidgetItem(p->ui->serviceTree);
	item->setText(0, server);
	Jreen::Disco::Item di;
	di.setJid(p->ui->serviceServer->currentText());
	item->setData(0, ItemRole, qVariantFromValue(di));
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
	if (!item)
		return;
	Jreen::Disco::Item di = item->data(0, ItemRole).value<Jreen::Disco::Item>();
	p->currentMenuItem = di;
	if (di.actions() & Jreen::Disco::Item::ActionJoin)
		p->contextMenu->addAction(p->ui->actionJoin);
	if (di.actions() & Jreen::Disco::Item::ActionRegister)
		p->contextMenu->addAction(p->ui->actionRegister);
	if (di.actions() & Jreen::Disco::Item::ActionSearch)
		p->contextMenu->addAction(p->ui->actionSearch);
	if (di.actions() & Jreen::Disco::Item::ActionExecute)
		p->contextMenu->addAction(p->ui->actionExecute);
	if (di.actions() & Jreen::Disco::Item::ActionAdd)
		p->contextMenu->addAction(p->ui->actionAdd);
	if (di.actions() & Jreen::Disco::Item::ActionVCard)
		p->contextMenu->addAction(p->ui->actionShowVCard);
//	if (di.actions() & Jreen::Disco::Item::ActionProxy)
//		p->contextMenu->addAction(p->ui->action);
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
	Jreen::Disco::Item di = item->data(0, ItemRole).value<Jreen::Disco::Item>();
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

QString JServiceBrowser::serviceIcon(const Jreen::Disco::Item &di)
{
	if (di.identities().isEmpty())
		return QString();
	QString service_icon;
	if (di.hasIdentity("server")) {
		service_icon = "network-server";
	} else if (di.hasIdentity("conference", "text")) {
		if (Jreen::JID(di.jid()).node().isEmpty())
			service_icon = "conference-server";
		else if (Jreen::JID(di.jid()).resource().isEmpty())
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
	return service_icon;
}

/*void JServiceBrowser::on_registerButton_clicked()
 {
  QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
  emit registerTransport(item->text(1));
 }

 void JServiceBrowser::on_searchFormButton_clicked()
 {
  QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
  Jreen::Disco::Item di;
  di = item->data(0, ItemRole).value<Jreen::Disco::Item>();
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
  Jreen::Disco::Item di;
  di = item->data(0, ItemRole).value<Jreen::Disco::Item>();
  emit showVCard(item->text(1));
 }

 void JServiceBrowser::on_addProxyButton_clicked()
 {
  QTreeWidgetItem *item = p->ui->serviceTree->currentItem();
  emit addProxy(JID(utils::toStd(item->text(1))));
 }*/

void JServiceBrowser::filterItem(const QString &mask)
{
	Q_UNUSED(mask);
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
	if (GroupChatManager *manager = p->account->groupChatManager()) {
		DataItem fields;
		fields.addSubitem(StringDataItem(QLatin1String("conference"), LocalizedString(),
		                               p->currentMenuItem.jid().full()));
		fields.addSubitem(StringDataItem(QLatin1String("nickname"), LocalizedString(),
		                               p->account->name()));
		manager->join(fields);
	}
}

void JServiceBrowser::onAddToRoster()
{
	ChatUnit *u = p->account->unit(p->currentMenuItem.jid(), true);
	if(Contact *c = qobject_cast<Contact*>(u))
		c->setInList(true);
}

}


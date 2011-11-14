/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright (C) 2011 Alexander Kazarin <boiler@co.ru>
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

#include "historywindow.h"
#include <QPoint>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileInfo>
#include <QDir>
#include "sqlhistory.h"
#include <qutim/iconmanagerinterface.h>
#include <QScrollBar>

namespace SqlHistoryNamespace {

HistoryWindow::HistoryWindow(const TreeModelItem &item, SqlEngine *engine, QWidget *parent)
	: QWidget(parent), m_item(item), m_engine(engine)
{
	ui.setupUi(this);
	
	ui.historyLog->setHtml("<p align='center'><span style='font-size:36pt;'>" 
			+ tr("No History") + "</span></p>");

  ui.label_in->setText(tr("In: %L1").arg(0));
  ui.label_out->setText(tr("Out: %L1").arg(0));
  ui.label_all->setText(tr("All: %L1").arg(0));


	SystemsCity::PluginSystem()->centerizeWidget(this);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
	QList<int> sizes;
	sizes.append(85);
	sizes.append(250);
	ui.splitter->setSizes(sizes);
	ui.splitter->setCollapsible(1,false);
	m_history_path = engine->getHistoryPath();
	setIcons();
	fillAccountComboBox();
	
	connect(ui.accountComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(fillContactComboBox(int)));
//	connect(ui.dateTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(fillMonth(QTreeWidgetItem*)));
	ui.dateTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.dateTreeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(contextMenuRequested(const QPoint&)));
	connect(ui.dateTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(itemClicked(QTreeWidgetItem*, int)));
	
	QStringList account_data; account_data<<m_item.protocol<<m_item.account;
	int account_index = ui.accountComboBox->findData(account_data);
	if ( !account_index )
		fillContactComboBox(0);
	else
		ui.accountComboBox->setCurrentIndex(account_index);
	
	connect(ui.fromComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(fillDateTreeWidget(int)));
	connect(ui.searchButton, SIGNAL(pressed()), this, SLOT(fillDateTreeWidget()));
	ui.searchButton->setCheckable(1);

// delete actions (macek)
	deleteCurrentAction = new QAction(tr("&Delete"), this);
	connect(deleteCurrentAction, SIGNAL(triggered()), this, SLOT(deleteCurrent()));
  
	deleteDayAction = new QAction(tr("&Delete day"), this);
	connect(deleteDayAction, SIGNAL(triggered()), this, SLOT(deleteDay()));

	deleteSelectedAction = new QAction(tr("&Delete  %L1 messages"), this);
	connect(deleteSelectedAction, SIGNAL(triggered()), this, SLOT(deleteSelectedItems()));
  
	clearSelectedAction = new QAction(tr("&Clear selection"), this);
	connect(clearSelectedAction, SIGNAL(triggered()), this, SLOT(clearSelectedItems()));

	QStringList contact_data;
	contact_data<<m_item.protocol<<m_item.account<<m_item.name;
	int from_index = ui.fromComboBox->findData(contact_data);
	if ( !from_index )
		fillDateTreeWidget(0);
	else
		ui.fromComboBox->setCurrentIndex(from_index);

	show();
}

void HistoryWindow::setIcons()
{
	setWindowIcon(Icon("history"));
	ui.searchButton->setIcon(Icon("search"));
}

void HistoryWindow::fillAccountComboBox()
{
	ui.accountComboBox->clear();
	QSqlQuery query("select protocol, account from history group by protocol, account");
	while (query.next()) {
		QString protocol_name = query.value(0).toString();
		Icon protocol_icon(protocol_name.toLower(), IconInfo::Protocol);
		QString account_name = query.value(1).toString();
		qutim_sdk_0_3::TreeModelItem item;
		item.protocol = protocol_name;
		item.account = account_name;
		item.name = account_name;
		item.type = 0;
		QString account_nickname = account_name; //macek: check before accessing!!
		if (SystemsCity::PluginSystem()->getAdditionalInfoAboutContact(item).count() > 0) {
			QStringList account_info = SystemsCity::PluginSystem()->getAdditionalInfoAboutContact(item);
			if (account_info.count()>0) account_nickname = account_info.at(0);
		}
		QStringList account_data; account_data<<protocol_name<<account_name;
		ui.accountComboBox->addItem(protocol_icon, account_nickname, account_data);
	}
}

void HistoryWindow::fillContactComboBox(int index)
{
	ui.fromComboBox->clear();
	if (index>-1) {
		QStringList account_data = ui.accountComboBox->itemData(index).toStringList();
		QString protocol = account_data.at(0);
		QString account = account_data.at(1);
		QSqlQuery query("select distinct(itemname) from history where protocol='"+protocol+"' and account='"+account+"'");
		while (query.next()) {
			QString item_name = query.value(0).toString();
			Icon protocol_icon(protocol.toLower(), IconInfo::Protocol);
			qutim_sdk_0_3::TreeModelItem item;
			item.protocol = protocol;
			item.account = account;
			item.name = item_name;
			item.type = 0;
			QString contact_name = item_name; //macek: check before accessing!!
			if (SystemsCity::PluginSystem()->getAdditionalInfoAboutContact(item).count() > 0) {
				QStringList contact_info = SystemsCity::PluginSystem()->getAdditionalInfoAboutContact(item);
				if (contact_info.count()>0) contact_name = contact_info.at(0);
			}
			QStringList item_data; item_data<<protocol<<account<<item_name;
			ui.fromComboBox->addItem(protocol_icon, contact_name, item_data);
		}
	}
}

void HistoryWindow::fillDateTreeWidget() {
  if (!ui.searchButton->isChecked()) {
  	m_search_word = ui.searchEdit->text();
	if (m_search_word.isEmpty()) {
		ui.searchButton->setChecked(1);
		return;
	}
  }
  if (!m_search_word.isEmpty() & ui.searchButton->isChecked() ) {
  	m_search_word = "";
  }
  ui.searchEdit->setEnabled(m_search_word.isEmpty()); 
  fillDateTreeWidget(ui.fromComboBox->currentIndex());
}



void HistoryWindow::fillDateTreeWidget(int index)
{
	ui.dateTreeWidget->clear();
	selectedItems.clear();
	if (index>-1) {
		QStringList item_data = ui.fromComboBox->itemData(index).toStringList();
		if (item_data.count()<3) return;
		QString protocol = item_data[0];
		QString account = item_data[1];
		QString itemname = item_data[2];
		QString like_clause = (!m_search_word.isEmpty())? " and text like '%" + m_search_word + "%'" : "";

		QSqlQuery query("select text, datetime from history where protocol='"+protocol+"' and account='"+account+"' and itemname='"+itemname+"'" + like_clause);
		QStringList years;
		QHash<QString, QStringList> months;
		while (query.next()) {
      //QString text = query.value(0).toString();
      //if (!m_search_word.isEmpty()) {
      //  if (text.indexOf(m_search_word, 0, Qt::CaseInsensitive) == -1)
      //    continue;
      //}
		QDateTime datetime = QDateTime::fromTime_t(query.value(1).toUInt());
			QString year = datetime.toString("yyyy");
			QString month = datetime.toString("MM");
			if (!years.contains(year)) years.append(year);
			if (!months[year].contains(month)) months[year].append(month);
		}

		QTreeWidgetItem *year_item=0;
		QTreeWidgetItem *month_item=0;
		QTreeWidgetItem *last_item=0;

		foreach (QString year, years) {
			year_item = new QTreeWidgetItem(ui.dateTreeWidget);
			year_item->setText(0, year);
			year_item->setIcon(0, Icon("year"));
			year_item->setExpanded(true);

			foreach (QString month, months[year]) {
				month_item = new QTreeWidgetItem(year_item);
				month_item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
				month_item->setText(0, QDate::longMonthName(month.toUInt(), QDate::StandaloneFormat));
				QStringList month_item_data;
				month_item_data<<protocol<<account<<itemname<<year<<month;
				month_item->setData(0, Qt::UserRole, month_item_data);
				if (month.toUInt() <= 2 || month.toUInt() == 12) month_item->setIcon(0, Icon("winter"));
				else if (month.toUInt() >= 3 && month.toUInt() <=5) month_item->setIcon(0, Icon("spring"));
				else if (month.toUInt() >= 6 && month.toUInt() <=8) month_item->setIcon(0, Icon("summer"));
				else if (month.toUInt() >= 9 && month.toUInt() <=11) month_item->setIcon(0, Icon("autumn"));
				fillMonth(month_item);
				if (month_item->childCount()>0)	{
					month_item->setExpanded(true);
					last_item = month_item->child(month_item->childCount() - 1);
				}
			}
		}
		if (last_item) {
			ui.dateTreeWidget->setCurrentItem(last_item);
		}
		else {
			ui.historyLog->setHtml("<p align='center'><span style='font-size:24pt;'>" + tr("Not found") + "</span></p>");
			ui.label_in->setText(tr("In: %L1").arg(0));
			ui.label_out->setText(tr("Out: %L1").arg(0));
			ui.label_all->setText(tr("All: %L1").arg(0));
		}

	}
}

void HistoryWindow::fillMonth(QTreeWidgetItem *month_item)
{
	if (month_item->childCount()) return;
	QStringList month_item_data = month_item->data(0, Qt::UserRole).toStringList();

	if (month_item_data.count()==5) {
		QString protocol = month_item_data[0];
		QString account = month_item_data[1];
		QString itemname = month_item_data[2];
		QString year = month_item_data[3];
		QString month = month_item_data[4];

		uint from_date_ts = QDateTime::fromString(year+"-"+month+"-01", "yyyy-MM-dd").toTime_t();
		uint to_year = year.toUInt();
		uint to_month = month.toUInt()+1;
		if (to_month>12) { to_month=1; to_year++; }
		uint to_date_ts = QDateTime::fromString(QString::number(to_year)+"-"+QString::number(to_month)+"-01", "yyyy-MM-dd").toTime_t();
		QString like_clause = (!m_search_word.isEmpty())? " and text like '%" + m_search_word + "%'" : "";
		QStringList days;
		QSqlQuery query("select text, datetime from history where protocol='"+protocol+"' and account='"+account+"' and itemname='"+itemname+"' and datetime>="+QString::number(from_date_ts)+" and datetime<"+QString::number(to_date_ts)+like_clause + " order by datetime");
		while (query.next()) {
    //  QString text = query.value(0).toString();
    //  if (!m_search_word.isEmpty()) {
    //    if (text.indexOf(m_search_word, 0, Qt::CaseInsensitive) == -1)
    //     continue;
    //    }
		QDateTime rowdate = QDateTime::fromTime_t(query.value(1).toUInt());
			QString rowday = rowdate.toString("dd");
			if (!days.contains(rowday)) days.append(rowday);
		}

		foreach (QString day, days) {
			QTreeWidgetItem *day_item = new QTreeWidgetItem(month_item);
			day_item->setText(0, day);
			day_item->setIcon(0, Icon("day"));
			QStringList day_item_data = month_item_data;
			day_item_data << day;
			day_item->setData(0, Qt::UserRole, day_item_data);
		}
	}
}

void HistoryWindow::on_dateTreeWidget_currentItemChanged( QTreeWidgetItem* current, QTreeWidgetItem* )
{
	if (current) {
		QStringList item_data = current->data(0, Qt::UserRole).toStringList();
		if (item_data.count()==6) {
			QString protocol = item_data[0];
			QString account = item_data[1];
			QString itemname = item_data[2];
			QString year = item_data[3];
			QString month = item_data[4];
			QString day = item_data[5];
			QString history_html;
			int in=0, out=0;
			uint from_date_ts = QDateTime::fromString(year+"-"+month+"-"+day+" 00:00:00", "yyyy-MM-dd HH:mm:ss").toTime_t();
			uint to_date_ts = QDateTime::fromString(year+"-"+month+"-"+day+" 23:59:59", "yyyy-MM-dd HH:mm:ss").toTime_t();

			qutim_sdk_0_3::TreeModelItem account_item;
			account_item.protocol = protocol;
			account_item.account = account;
			account_item.name = account;
			account_item.type = 0;
			QString account_nickname = account; //macek: check before accessing!!
			if (SystemsCity::PluginSystem()->getAdditionalInfoAboutContact(account_item).count() > 0) {
				account_nickname = SystemsCity::PluginSystem()->getAdditionalInfoAboutContact(account_item).at(0);
			}

			qutim_sdk_0_3::TreeModelItem contact_item;
			contact_item.protocol = protocol;
			contact_item.account = account;
			contact_item.name = itemname;
			contact_item.type = 0;

			QString contact_nickname = itemname; //macek: check before accessing!!
			if (SystemsCity::PluginSystem()->getAdditionalInfoAboutContact(contact_item).count() > 0) {
				contact_nickname = SystemsCity::PluginSystem()->getAdditionalInfoAboutContact(contact_item).at(0);
			}
			QString like_clause = (!m_search_word.isEmpty())? " and text like '%" + m_search_word + "%'" : "";

			QSqlQuery query("select datetime, text, direction, id from history where protocol='"+protocol+"' and account='"+account+"' and itemname='"+itemname+"' and datetime>="+QString::number(from_date_ts)+" and datetime<="+QString::number(to_date_ts) + like_clause +" order by datetime");
			//macek: remove children
			int chCount = current->childCount();

			while (query.next()) {
				QDateTime datetime = QDateTime::fromTime_t(query.value(0).toUInt());
				QString message = query.value(1).toString();
				uint direction = query.value(2).toUInt();
				history_html += (direction==0) ? "<b><font color='red'>" : "<b><font color='blue'>";
				history_html += (direction==0) ? account_nickname : contact_nickname;
				history_html += " ( "+datetime.time().toString()+" )</font></b><br />";
				history_html += "<a id='" + query.value(3).toString()+ "'>";
				int pos = message.indexOf(m_search_word, 0, Qt::CaseInsensitive);
				if (!m_search_word.isEmpty() && (pos != -1)) {
					QString tmp = message.mid(pos, m_search_word.length());
					history_html += message.replace(tmp, "<font style=\"background-color: yellow\">"+tmp+"</font>");
				} else history_html += message;
				history_html += "</a><br /><br />";
				//macek: count incoming and outgoing messages
				if (direction==0) in++;
				else out++;
				if (chCount == 0) {
				//macek: fill message items
					QTreeWidgetItem *msg_item = new QTreeWidgetItem(current);
					QString msg_header = datetime.time().toString() ;
					QString msg_key = ((direction==0) ? account_nickname : contact_nickname) + " ( "+datetime.time().toString()+" )";
					msg_item->setText(0, msg_header);
					//msg_item->setIcon(0, Icon("direction")); //todo: change icon according to incoming/outgoing type
					msg_item->setCheckState(0, Qt::Unchecked);
					QStringList msg_item_data = item_data;
					QColor item_color = (direction==0) ? Qt::red : Qt::blue;
					msg_item->setForeground(0, QBrush(item_color));
					msg_item_data << msg_key;
					msg_item_data << query.value(3).toString();
					msg_item_data << QString(message.length());
					msg_item->setData(0, Qt::UserRole, msg_item_data);
				}
			}
			ui.historyLog->setHtml(history_html);
 
			// if (!m_search_word.isEmpty())
			//   ui.historyLog->find(m_search_word);
      
			//macek: display counters
			ui.label_in->setText( QString(tr( "In: %L1")).arg( in ) );
			ui.label_out->setText( tr( "Out: %L1").arg( out ) );
			ui.label_all->setText( tr( "All: %L1").arg( in + out ) );
		}
		else if (item_data.count()==9) {
			QString msg_key = item_data[6];
			if (!ui.historyLog->find(msg_key))
				if (!ui.historyLog->find(msg_key, QTextDocument::FindBackward))
					on_dateTreeWidget_currentItemChanged(current->parent(), NULL);
		}

	}
}


void HistoryWindow::itemClicked(QTreeWidgetItem * item, int  )
{
 if (item) {
    QStringList item_data = item->data(0, Qt::UserRole).toStringList();
    if (item_data.count()==9) {
        //add checked item to list
     if (item->checkState(0) == Qt::Checked)
     {  
          if (!selectedItems.contains(item))
 	          selectedItems.append(item);
     } else
     { 
          if (selectedItems.contains(item))
 	          selectedItems.removeAll(item);
     }
		}
	}
}

//delete items menu
void HistoryWindow::contextMenuRequested (const QPoint &pos)
{

 QTreeWidgetItem* item = ui.dateTreeWidget->currentItem();

 if (item)
 {
    QStringList item_data = item->data(0, Qt::UserRole).toStringList();
    
    if (item_data.count()==6) {
	QMenu *menu = new QMenu;

    	QString month = item_data[4];
    	QString day = item_data[5];
    	QString monthName = QDate::longMonthName(month.toUInt(), QDate::StandaloneFormat); 

 	deleteDayAction->setText(tr("Delete &")  + " " + monthName + ", " + day);
  	menu->addAction(deleteDayAction);
	menu->exec(ui.dateTreeWidget->mapToGlobal(pos));
   } else	
    if (item_data.count()==9) {
    	QString month = item_data[4];
    	QString day = item_data[5];
    	QString monthName = QDate::longMonthName(month.toUInt(), QDate::StandaloneFormat); 

	QMenu *menu = new QMenu;
 	deleteDayAction->setText(tr("Delete &") + " " + monthName + ", " + day);
  	menu->addAction(deleteDayAction);

        if (selectedItems.count() > 0 ) { 
		deleteSelectedAction->setText(tr("Delete %L1 &messages").arg(selectedItems.count()));
  		menu->addAction(deleteSelectedAction);
  		menu->addAction(clearSelectedAction);
	}
	else
	{
		deleteCurrentAction->setText(tr("&Delete") + " " + item_data[6]);  	
		menu->addAction(deleteCurrentAction);
	}	
	menu->exec(ui.dateTreeWidget->mapToGlobal(pos));
   }	
 } 
}


void HistoryWindow::deleteCurrent()
{
 QTreeWidgetItem* item = ui.dateTreeWidget->currentItem();
 if (item)
 {
    QStringList item_data = item->data(0, Qt::UserRole).toStringList();
    //build WHERE clause
    QString where_clause = "id=" + item_data[7];
    QSqlQuery query;
    query.prepare("delete from history where " + where_clause ); 
    query.exec();

    QTreeWidgetItem *  parent = item->parent();
    on_dateTreeWidget_currentItemChanged(parent, NULL);
    // delete item from tree	  
    if (parent!=NULL)  {
	parent->removeChild(item);
    	//remove parent node if empty
 	if (parent->childCount() == 0)
 	{
		QTreeWidgetItem * grandParent = parent->parent();
		if (grandParent!=NULL)
			 grandParent->removeChild(parent);
		parent = grandParent;

        } 
   }
 }
}


void HistoryWindow::clearSelectedItems()
{
 foreach(QTreeWidgetItem * item, selectedItems){
  item->setCheckState(0, Qt::Unchecked);
 } 
 selectedItems.clear();
}


void HistoryWindow::deleteSelectedItems()
{
 if (selectedItems.count()==0) return;

 QString in_clause = "";
 QTreeWidgetItem * parent=NULL;
 QSqlQuery query;


 //build IN clause
 foreach(QTreeWidgetItem * item, selectedItems){
    QStringList item_data = item->data(0, Qt::UserRole).toStringList();
    in_clause += (in_clause == "") ? item_data[7] : ", " + item_data[7];
 }	
 //remove from database 
 query.prepare("delete from history where id in (" + in_clause + ")"); 
 query.exec();

 //remove from tree
 foreach(QTreeWidgetItem * item, selectedItems){
    if (parent!= item->parent()){
    	parent = item->parent();
 	on_dateTreeWidget_currentItemChanged(parent, NULL);
    }
    if (parent!=NULL) {
	 parent->removeChild(item);
    	//remove parent node if empty
 	if (parent->childCount() == 0)
 	{
		QTreeWidgetItem * grandParent = parent->parent();
		if (grandParent!=NULL)
			 grandParent->removeChild(parent);
        } 
      }	
 }	

 selectedItems.clear();
}


void HistoryWindow::deleteDay()
{
 QTreeWidgetItem* item = ui.dateTreeWidget->currentItem();
 if (item)
 {
    QStringList item_data = item->data(0, Qt::UserRole).toStringList();
    QString year = item_data[3];
    QString month = item_data[4];
    QString day = item_data[5];
    uint from_date_ts = QDateTime::fromString(year+"-"+month+"-"+day+" 00:00:00", "yyyy-MM-dd HH:mm:ss").toTime_t();
    uint to_date_ts = QDateTime::fromString(year+"-"+month+"-"+day+" 23:59:59", "yyyy-MM-dd HH:mm:ss").toTime_t();

    //build WHERE clause
    QString where_clause = "datetime>="+QString::number(from_date_ts)+" and datetime<="+QString::number(to_date_ts);
    // Delete only filtered, if filter active
    QString like_clause = (!m_search_word.isEmpty())? " and text like '%" + m_search_word + "%'" : ""; 

    QSqlQuery query;
    query.prepare("delete from history where " + where_clause + like_clause); 
    query.exec();

    //store parent to select after delete
    QTreeWidgetItem *  parent = NULL;
    if (item_data.count()==9) 
 	parent = item->parent();
    else
 	parent = item;

   //remove all children from tree and selection list	
   if (parent!=NULL)  {
	foreach(QTreeWidgetItem * child, parent->takeChildren())     
 		selectedItems.removeAll(child);
        item = parent;
 	parent = item->parent();
    }
    
   if (parent!=NULL)  parent->removeChild(item);

 }

}


}


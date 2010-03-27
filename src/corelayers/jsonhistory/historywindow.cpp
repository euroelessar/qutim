/*
    HistoryWindow

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>
				  2009 by Ruslan Nigmatullin <euroelessar@gmail.com>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "historywindow.h"
#include <QPoint>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileInfo>
#include <QDir>
#include "jsonhistory.h"
#include "libqutim/icon.h"
#include "libqutim/systeminfo.h"
#include "libqutim/protocol.h"
#include "libqutim/account.h"
#include "libqutim/chatunit.h"
#include <QScrollBar>
#include "libqutim/json.h"
#include <QDebug>

namespace Core
{
HistoryWindow::HistoryWindow(const ChatUnit *unit) : m_unit(unit)
{
	ui.setupUi(this);
	
	ui.historyLog->setHtml("<p align='center'><span style='font-size:36pt;'>" 
			+ tr("No History") + "</span></p>");
	ui.label_in->setText( tr( "In: %L1").arg( 0 ) );
	ui.label_out->setText( tr( "Out: %L1").arg( 0 ) );
	ui.label_all->setText( tr( "All: %L1").arg( 0 ) );

	centerizeWidget(this);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
	QList<int> sizes;
	sizes.append(80);
	sizes.append(250);
	ui.splitter->setSizes(sizes);
	ui.splitter->setCollapsible(1,false);
	m_history_path = SystemInfo::getPath(SystemInfo::HistoryDir);
	setIcons();
	fillAccountComboBox();
	
	connect(ui.accountComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(fillContactComboBox(int)));
	connect(ui.dateTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(fillMonth(QTreeWidgetItem*)));

	int accountIndex = ui.accountComboBox->findData(m_unit->account()->protocol()->id()
													+ "."
													+ JsonHistory::quote(m_unit->account()->id()));
	if ( !accountIndex )
		fillContactComboBox(0);
	else
		ui.accountComboBox->setCurrentIndex(accountIndex);
	
	connect(ui.fromComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(fillDateTreeWidget(int)));
	
	int contactIndex = ui.fromComboBox->findData(m_unit->id());
	if ( !contactIndex )
		fillDateTreeWidget(0);
	else
		ui.fromComboBox->setCurrentIndex(contactIndex);
	show();
}

void HistoryWindow::setUnit(const ChatUnit *unit)
{
	m_unit = unit;
	int accountIndex = ui.accountComboBox->findData(m_unit->account()->protocol()->id()
													+ "."
													+ JsonHistory::quote(m_unit->account()->id()));
	if (accountIndex > -1) {
		ui.accountComboBox->setCurrentIndex(accountIndex);
		int contactIndex = ui.fromComboBox->findData(m_unit->id());
		if (!contactIndex) {
			fillDateTreeWidget(0);
		} else {
			ui.fromComboBox->setCurrentIndex(contactIndex);
			return;
		}
	}
	fillContactComboBox(0);
	ui.historyLog->setHtml("<p align='center'><span style='font-size:36pt;'>"
						   + tr("No History") + "</span></p>");
}

void HistoryWindow::setIcons()
{
//	setWindowIcon(Icon("history"));
//	ui.searchButton->setIcon(Icon("search"));
}

void HistoryWindow::fillAccountComboBox()
{
	QDir history_dir(m_history_path);
	QStringList accounts = history_dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
	const QStringList filter = QStringList() << "*.*.json";
	foreach (QString account, accounts) {
		QDir account_dir = history_dir.filePath(account);
		if (account_dir.entryList(filter).isEmpty())
			continue;
		QString protoId = account.section(".",0,0);
		QString accountId = JsonHistory::unquote(account.section(".",1));
		Icon protoIcon(QLatin1String("im-") + protoId);
		QString accountName = accountId;
		if (Protocol *protocol = allProtocols().value(protoId)) {
			if (Account *acc = protocol->account(accountId)) {
				QString name = acc->name();
				if (!name.isEmpty() && name != acc->id())
					accountName += " - " + name;
			}
		}
		ui.accountComboBox->addItem(protoIcon, accountName, account);
	}
}

void HistoryWindow::fillContactComboBox(int index)
{
	if ( ui.accountComboBox->count() > 0 ) {
		QString item_data = ui.accountComboBox->itemData(index).toString();
		QString protoId = item_data.section(".",0,0);
		QString accountId = JsonHistory::unquote(item_data.section(".",1,-1));
		QDir account_history_dir(m_history_path + "/" + item_data);
		QStringList log_list = account_history_dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
		ui.fromComboBox->clear();
		foreach (QString contact, log_list) {
			QString contactId = JsonHistory::unquote(contact.section(".",0,-3));
			if ( ui.fromComboBox->findData(contactId) != -1 )
				continue;
			QString contactName = contactId;
			if (Protocol *protocol = allProtocols().value(protoId)) {
				if (Account *acc = protocol->account(accountId)) {
					if (ChatUnit *unit = acc->getUnit(contactId)) {
						QString name = unit->title();
						if (!name.isEmpty() && name != unit->id())
							contactName += " - " + name;
					}
				}
			}
			ui.fromComboBox->addItem(contactName, contactId);
		}
	}
	ui.fromComboBox->model()->sort( 0 );
}

void HistoryWindow::fillDateTreeWidget(int index, const QString &search_word)
{
	m_search_word = search_word;
	if ( ui.fromComboBox->count() > 0 )
	{
		int account_index = ui.accountComboBox->currentIndex();
		if ( account_index < 0 )
			return;
		QString item_data = ui.accountComboBox->itemData(account_index).toString();
		QDir account_history_dir(m_history_path + "/" + item_data);
		QStringList filters;
		QString from_name = ui.fromComboBox->itemData(index).toString();
		filters<<JsonHistory::quote(from_name) + ".*";
		QStringList from_files = account_history_dir.entryList(filters,
				QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
		int year = 0, month = 0;
		QString date;
		ui.dateTreeWidget->clear();
		QTreeWidgetItem *year_item = 0;
		QTreeWidgetItem *month_item = 0;
		QTreeWidgetItem *last_item = 0;
		foreach(QString history_file, from_files)
		{
			date = history_file.section( '.', -2, -2 );
			if( date.length() != 6 )
				continue;
			int tmp_year = date.mid(0,4).toInt();
			int tmp_month = date.mid(4,2).toInt();
			if( year != tmp_year || !year_item )
			{
				year = tmp_year;
				year_item = new QTreeWidgetItem(ui.dateTreeWidget);
				year_item->setText(0, date.mid(0,4));
				year_item->setIcon(0, Icon("view-calendar-year"));
				year_item->setExpanded(true);
			}
			if( tmp_month != month )
			{
				month = tmp_month;
				month_item = new QTreeWidgetItem(year_item);
				month_item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
				month_item->setText(0, QDate::longMonthName(month));
				month_item->setData(0, Qt::UserRole, account_history_dir.absoluteFilePath(history_file));
				month_item->setExpanded(false);
				month_item->setIcon(0, Icon("view-calendar-month"));
//				if ( month <= 2 || month == 12 )
//					month_item->setIcon(0, Icon("winter"));
//				else if ( month >= 3 && month <=5 )
//					month_item->setIcon(0, Icon("spring"));
//				else if ( month >= 6 && month <=8 )
//					month_item->setIcon(0, Icon("summer"));
//				else if ( month >= 9 && month <=11 )
//					month_item->setIcon(0, Icon("autumn"));
			}
		}
		if( month_item )
		{
			fillMonth(month_item);
			if( month_item->childCount() > 0 )
			{
				month_item->setExpanded(true);
				last_item = month_item->child(month_item->childCount() - 1);
			}
		}
		if ( last_item )
			ui.dateTreeWidget->setCurrentItem(last_item);
		setWindowTitle(QString("%1(%2)").arg(ui.fromComboBox->currentText())
				.arg(ui.accountComboBox->currentText()));
	}
}

void HistoryWindow::fillMonth(QTreeWidgetItem *month_item)
{
	QString file_path = month_item->data(0, Qt::UserRole).toString();
	if( month_item->childCount() || file_path.isEmpty() )
		return;
	QFile file(file_path);
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMap<int, QDateTime> days_list;
		int len = file.size();
		QByteArray data;
		const uchar *fmap = file.map(0, file.size());
		if(!fmap)
		{
			data = file.readAll();
			fmap = (uchar *)data.constData();
		}
		const uchar *s = Json::skipBlanks(fmap, &len);
		QVariant val;
		uchar qch = *s;
		if(!s || (qch != '[' && qch != '{'))
			return;
		qch = (qch == '{' ? '}' : ']');
		s++;
		len--;
		bool first = true;
		QDateTime history_date_time;
		QString history_message;
		QRegExp search_regexp("(" + QRegExp::escape(m_search_word) + ")", Qt::CaseInsensitive);
		while(s)
		{
			val.clear();
			s = Json::skipBlanks(s, &len);
			if(len < 2 || (s && *s == qch))
				break;
			if((!first && *s != ',') || (first && *s == ','))
				break;
			first = false;
			if(*s == ',')
			{
				s++;
				len--;
			}
			if(!(s = Json::parseRecord(val, s, &len)))
				break;
			else
			{
				QVariantMap message = val.toMap();
				history_date_time = QDateTime::fromString(message.value("datetime").toString(), Qt::ISODate);
				history_message = message.value("text").toString();
				int day = history_date_time.date().day();
				if ( ( m_search_word.isEmpty()
						&& !days_list.contains(day) )
						|| ( !m_search_word.isEmpty()
						&& !days_list.contains(day)
						&& ( history_message.contains(search_regexp) ) ))
				{
					days_list.insert(day,history_date_time);
				}
			}
		}
		file.close();
		if ( days_list.count() )
		{
			int day = -1;
			foreach(const QDateTime &history_date, days_list)
			{
				int tmp_day = history_date.date().day();
				if ( tmp_day != day )
				{
					day = tmp_day;
					QTreeWidgetItem *day_item = new QTreeWidgetItem(month_item);
					day_item->setText(0, QString::number(day) +
							history_date.time().toString("(hh:mm)"));
					day_item->setIcon(0, Icon("day"));
					day_item->setData(0, Qt::UserRole, history_date);
				}
			}
		}
	}
}

void HistoryWindow::on_dateTreeWidget_currentItemChanged( QTreeWidgetItem* current, QTreeWidgetItem* )
{
	if ( current )
	{
		QDateTime item_date_time = current->data(0, Qt::UserRole).toDateTime();
		if ( !item_date_time.isNull() )
		{
			QString search_word = ui.searchEdit->text();
			QRegExp search_regexp("(" + QRegExp::escape(search_word) + ")", Qt::CaseInsensitive);
			int account_index = ui.accountComboBox->currentIndex();
			int from_index = ui.fromComboBox->currentIndex();
			if ( account_index < 0 || from_index < 0)
				return;
			int in=0, out=0;
			QDir account_history_dir(m_history_path + "/" + 
					ui.accountComboBox->itemData(account_index).toString());
			QFile file(account_history_dir.absoluteFilePath(
					JsonHistory::quote(ui.fromComboBox->itemData(from_index).toString()) +
					"." + item_date_time.date().toString("yyyyMM") + ".json"));
			if (file.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				int len = file.size();
				QByteArray data;
				const uchar *fmap = file.map(0, file.size());
				if(!fmap)
				{
					data = file.readAll();
					fmap = (uchar *)data.constData();
				}
				const uchar *s = Json::skipBlanks(fmap, &len);
				QVariant val;
				uchar qch = *s;
				if(!s || (qch != '[' && qch != '{'))
					return;
				qch = (qch == '{' ? '}' : ']');
				s++;
				len--;
				bool first = true;
				int day = item_date_time.date().day();
				QString account_nickname = ui.accountComboBox->currentText();
				if ( account_nickname.contains( "-" ) )
					account_nickname.remove( QRegExp( ".*-\\s" ) );
				QString from_nickname = ui.fromComboBox->currentText();
				if ( from_nickname.contains( "-" ) )
					from_nickname.remove( QRegExp( ".*-\\s" ) );
				QString history_html;
				QDateTime history_date_time;
				bool history_in;
				QString history_message;
				while(s)
				{
					val.clear();
					s = Json::skipBlanks(s, &len);
					if(len < 2 || (s && *s == qch))
						break;
					if((!first && *s != ',') || (first && *s == ','))
						break;
					first = false;
					if(*s == ',')
					{
						s++;
						len--;
					}
					if(!(s = Json::parseRecord(val, s, &len)))
						break;
					else
					{
						QVariantMap message = val.toMap();
						history_date_time = QDateTime::fromString(message.value("datetime").toString(), Qt::ISODate);
						history_in = message.value("in", false).toBool();
						history_message = message.value("text").toString();

						if ( history_date_time.date().day() == day )
						{
							QString history_html_2;
							history_in ? in++ : out++;
							history_html_2 += history_in ? "<b><font color='blue'>" : "<b><font color='red'>";
							history_html_2 += history_in ? from_nickname : account_nickname;
							history_html_2 += " ( ";
							history_html_2 += history_date_time.time().toString();
							history_html_2 += " )</font></b><br>";
							if ( search_word.isEmpty() )
							{
								history_html_2 += history_message;
								history_html_2 += "<br>";
							}
							else
							{
								static QString result( "<font style=background-color:yellow>\\1</font>" );
								history_html_2 += "<font style=background-color:white>";
								history_html_2 += history_message.replace(search_regexp, result);
								history_html_2 += "</font><br>";
							}
							history_html += history_html_2;
						}
					}
				}
				file.close();
				ui.historyLog->setHtml(history_html);
				if(search_word.isEmpty())
					ui.historyLog->moveCursor(QTextCursor::End);
				else
					ui.historyLog->find(search_word);
				ui.historyLog->setLineWrapColumnOrWidth(ui.historyLog->lineWrapColumnOrWidth());
				ui.historyLog->ensureCursorVisible();
			}
			ui.label_in->setText( tr( "In: %L1").arg( in ) );
			ui.label_out->setText( tr( "Out: %L1").arg( out ) );
			ui.label_all->setText( tr( "All: %L1").arg( in + out ) );
		}
	}
}

void HistoryWindow::on_searchButton_clicked()
{
	if ( ui.accountComboBox->count() && ui.fromComboBox->count() )
	{
		if( m_search_word == ui.searchEdit->text() )
		{
			if(!ui.historyLog->find(m_search_word))
			{
				ui.historyLog->moveCursor(QTextCursor::Start);
				ui.historyLog->find(m_search_word);
				ui.historyLog->ensureCursorVisible();
			}
		}
		else
			fillDateTreeWidget(ui.fromComboBox->currentIndex(), ui.searchEdit->text().toLower());
	}
}

}

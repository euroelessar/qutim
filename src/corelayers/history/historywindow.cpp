/*
    HistoryWindow

    Copyright (c) 2008 by Rustam Chakin <qutim.develop@gmail.com>

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
#include "historylayer.h"
#include "../iconmanager.h"
#include "../pluginsystem.h"
#include <QScrollBar>

HistoryWindow::HistoryWindow(const QString &protocol_name,
		const QString &account_name,
		const QString &item_name,
		const QString &profile_name,
				AbstractHistoryLayer *h_layer,
		QWidget *parent)
    : QWidget(parent),
    m_protocol_name(protocol_name),
    m_account_name(account_name),
    m_profile_name(profile_name),
    m_h_layer(h_layer)
{
        TreeModelItem item;
        item.m_account_name = account_name;
        item.m_protocol_name = protocol_name;
        item.m_item_name = item_name;
	m_item_name = m_h_layer->getContactHistoryName(item);
	ui.setupUi(this);
	
	ui.historyLog->setHtml("<p align='center'><span style='font-size:36pt;'>" 
			+ tr("No History") + "</span></p>");
	
	codec = QTextCodec::codecForName("UTF-8");
	moveToDesktopCenter();
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_DeleteOnClose, true);
	QList<int> sizes;
	sizes.append(80);
	sizes.append(250);
	ui.splitter->setSizes(sizes);
	ui.splitter->setCollapsible(1,false);
	QSettings settings(QSettings::defaultFormat(), QSettings::UserScope, "qutim/qutim."+profile_name, "profilesettings");
	m_history_path = QFileInfo(settings.fileName()).absolutePath()+"/history/";
	setIcons();
	fillAccountComboBox();
	
	connect(ui.accountComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(fillContactComboBox(int)));
	connect(ui.dateTreeWidget, SIGNAL(itemExpanded(QTreeWidgetItem*)), this, SLOT(fillMonth(QTreeWidgetItem*)));
	
	int account_index = ui.accountComboBox->findData(
			protocol_name + "." + toHex(account_name));
	if ( !account_index )
		fillContactComboBox(0);
	else
		ui.accountComboBox->setCurrentIndex(account_index);
	
	connect(ui.fromComboBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(fillDateTreeWidget(int)));
	
        int from_index = ui.fromComboBox->findData(m_item_name);
	if ( !from_index )
		fillDateTreeWidget(0);
	else
		ui.fromComboBox->setCurrentIndex(from_index);
}

HistoryWindow::~HistoryWindow()
{
	m_h_layer->closeHistoyWindow(QString("%1.%2.%3").arg(m_protocol_name)
			.arg(m_account_name).arg(m_item_name));
}

void HistoryWindow::moveToDesktopCenter()
{
	PluginSystem::instance().centerizeWidget(this);
}

void HistoryWindow::setIcons()
{
	IconManager &im = IconManager::instance();
	setWindowIcon(im.getIcon("history"));
	ui.searchButton->setIcon(im.getIcon("search"));
}

void HistoryWindow::fillAccountComboBox()
{
	QDir history_dir(m_history_path);
	QStringList accounts = history_dir.entryList(QDir::AllDirs | 
			QDir::NoDotAndDotDot);
	PluginSystem &ps = PluginSystem::instance();
	foreach(QString account, accounts)
	{
		QString protocol = account.section(".",0,0);
		QString account_name = fromHex(account.section(".",1,1));
		Icon protocol_icon(protocol.toLower(), IconInfo::Protocol);
		TreeModelItem item;
		item.m_protocol_name = protocol;
		item.m_account_name = account_name;
		item.m_item_name = account_name;
		item.m_item_type = 0;
		QStringList account_info = ps.getAdditionalInfoAboutContact(item);
		QString account_nickname = account_name;
		if ( account_info.count() > 0 )
		{
			account_nickname = account_info.at(0);
		}
		ui.accountComboBox->addItem(protocol_icon,
				account_nickname, account);
	}
}

void HistoryWindow::fillContactComboBox(int index)
{
	if ( ui.accountComboBox->count() > 0 )
	{
		QString item_data = ui.accountComboBox->itemData(index).toString();
		QString protocol = item_data.section(".",0,0);
		QString account_name = fromHex(item_data.section(".",1,-1));
		QDir account_history_dir(m_history_path + "/" + item_data);
		QStringList log_list = account_history_dir.entryList(QDir::Files | QDir::NoDotAndDotDot);
		ui.fromComboBox->clear();
		foreach(QString contact, log_list)
		{
			QString contact_name = fromHex(contact.section(".",0,0));
			if ( ui.fromComboBox->findData(contact_name) != -1 )
				continue;
			TreeModelItem item;
			item.m_protocol_name = protocol;
			item.m_account_name = account_name;
			item.m_item_name = contact_name;
			item.m_item_type = 0;
			QStringList contact_info = 
				PluginSystem::instance().getAdditionalInfoAboutContact(item);
			QString contact_nickname = contact_name;
			if ( contact_info.count() > 0 )
			{
				contact_nickname = contact_info.at(0);
			}
			ui.fromComboBox->addItem(contact_nickname,
					contact_name);
		}
	}
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
		filters<<toHex(from_name) + ".*";
		QStringList from_files = account_history_dir.entryList(filters,
				QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
		int year = 0, month = 0, day = 0;
		QString date;
		ui.dateTreeWidget->clear();
		IconManager &im = IconManager::instance();
		QTreeWidgetItem *year_item = 0;
		QTreeWidgetItem *month_item = 0;
		QTreeWidgetItem *last_item = 0;
		foreach(QString history_file, from_files)
		{
			date = history_file.section( '.', 1, 1 );
			if( date.length() != 6 )
				continue;
			int tmp_year = date.mid(0,4).toInt();
			int tmp_month = date.mid(4,2).toInt();
			if( year != tmp_year || !year_item )
			{
				year = tmp_year;
				year_item = new QTreeWidgetItem(ui.dateTreeWidget);
				year_item->setText(0, date.mid(0,4));
				year_item->setIcon(0, Icon("year"));
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
				if ( month <= 2 || month == 12 )
					month_item->setIcon(0, Icon("winter"));
				else if ( month >= 3 && month <=5 )
					month_item->setIcon(0, Icon("spring"));
				else if ( month >= 6 && month <=8 )
					month_item->setIcon(0, Icon("summer"));
				else if ( month >= 9 && month <=11 )
					month_item->setIcon(0, Icon("autumn"));
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
	if (file.open(QIODevice::ReadOnly))
	{
		QMap<int, QDateTime> days_list;
		QDataStream in(&file);
		QRegExp search_regexp("(" + QRegExp::escape(m_search_word) + ")", Qt::CaseInsensitive);
		while(!file.atEnd())
		{
			QDateTime history_date_time;
			qint8 history_type;
			bool history_in;
			QString history_message;
			in		>> history_date_time
					>> history_type
					>> history_in
					>> history_message;
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

void HistoryWindow::on_dateTreeWidget_currentItemChanged( QTreeWidgetItem* current, QTreeWidgetItem* previous )
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
			QDir account_history_dir(m_history_path + "/" + 
					ui.accountComboBox->itemData(account_index).toString());
			QFile file(account_history_dir.absoluteFilePath(
					toHex(ui.fromComboBox->itemData(from_index).toString()) +
					"." + item_date_time.date().toString("yyyyMM") + ".log"));
			if (file.open(QIODevice::ReadOnly))
			{
				int day = item_date_time.date().day();
				QDataStream in(&file);
				QString account_nickname = ui.accountComboBox->currentText();
				QString from_nickname = ui.fromComboBox->currentText();
				QString history_html;
				while(!file.atEnd())
				{
					QDateTime history_date_time;
					qint8 history_type;
					bool history_in;
					QString history_message;
					in >> history_date_time >>
						history_type >>
						history_in >>
						history_message;
					if ( history_date_time.date().day() == day )
					{
						QString history_html_2;
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
				ui.historyLog->setHtml(history_html);
				if(search_word.isEmpty())
					ui.historyLog->moveCursor(QTextCursor::End);
				else
					ui.historyLog->find(search_word);
				ui.historyLog->setLineWrapColumnOrWidth(ui.historyLog->lineWrapColumnOrWidth());
				ui.historyLog->ensureCursorVisible();
			}
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

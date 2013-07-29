/****************************************************************************
**
** qutIM - instant messenger
**
** Copyright © 2008 Rustam Chakin <qutim.develop@gmail.com>
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

#include "historywindow.h"
#include <QPoint>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileInfo>
#include <QDir>
#include "jsonhistory.h"
#include <qutim/icon.h>
#include <qutim/systeminfo.h>
#include <qutim/protocol.h>
#include <qutim/account.h>
#include <qutim/chatunit.h>
#include <QScrollBar>
#include <qutim/json.h>
#include <QStringBuilder>
#include <QDebug>
#include <qutim/systemintegration.h>
#include <qutim/shortcut.h>

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
	Shortcut *shortcut = new Shortcut("findNext", this);
	connect(shortcut, SIGNAL(activated()), ui.searchButton, SLOT(click()));
	shortcut = new Shortcut("findPrevious", this);
	connect(shortcut, SIGNAL(activated()), SLOT(findPrevious()));

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
	setParent(QApplication::activeWindow());
#ifdef Q_WS_MAEMO_5
	setAttribute(Qt::WA_Maemo5StackedWindow);
#endif
	setWindowFlags(windowFlags() | Qt::Window);

	QAction *action = new QAction(tr("Close"),this);
	connect(action, SIGNAL(triggered()), SLOT(close()));
	addAction(action);
	SystemIntegration::show(this);
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
		if (Protocol *protocol = Protocol::all().value(protoId)) {
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
			if (Protocol *protocol = Protocol::all().value(protoId)) {
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
			QRegExp searchRegexp("(" + QRegExp::escape(search_word) + ")", Qt::CaseInsensitive);
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
					len = data.size();
				}
//				const uchar *s = Json::skipBlanks(fmap, &len);
//				QVariant val;
//				uchar qch = *s;
//				if(!s || (qch != '[' && qch != '{'))
//					return;
//				qch = (qch == '{' ? '}' : ']');
//				s++;
//				len--;
//				bool first = true;
				int day = item_date_time.date().day();
				QString account_nickname = ui.accountComboBox->currentText();
				if ( account_nickname.contains( "-" ) )
					account_nickname.remove( QRegExp( ".*-\\s" ) );
				QString from_nickname = ui.fromComboBox->currentText();
				if ( from_nickname.contains( "-" ) )
					from_nickname.remove( QRegExp( ".*-\\s" ) );
				bool history_service;
				QDateTime history_date_time;
				bool history_in;
				QString historyMessage;
				QTextDocument *doc = ui.historyLog->document();
				doc->setParent(0);
				ui.historyLog->setDocument(0);
				doc->clear();
				QTextCursor cursor = QTextCursor(doc);
				QTextCharFormat defaultFont = cursor.charFormat();
				QTextCharFormat serviceFont = cursor.charFormat();
				serviceFont.setForeground(Qt::darkGreen);
				serviceFont.setFontWeight(QFont::Bold);
				QTextCharFormat incomingFont = cursor.charFormat();
				incomingFont.setForeground(Qt::red);
				incomingFont.setFontWeight(QFont::Bold);
				QTextCharFormat outgoingFont = cursor.charFormat();
				outgoingFont.setForeground(Qt::blue);
				outgoingFont.setFontWeight(QFont::Bold);
//				QTextCharFormat foundFont = cursor.charFormat();
//				outgoingFont.setBackground(Qt::yellow);
				QString serviceMessageTitle = tr("Service message");
				QString resultString( "<span style='background: #ffff00'>\\1</span>" );
				cursor.beginEditBlock();
//				while (s) {
//					val.clear();
//					s = Json::skipBlanks(s, &len);
//					if(len < 2 || (s && *s == qch))
//						break;
//					if((!first && *s != ',') || (first && *s == ','))
//						break;
//					first = false;
//					if(*s == ',')
//					{
//						s++;
//						len--;
//					}
//					if(!(s = Json::parseRecord(val, s, &len)))
//						break;
//					else
				QVariant log;
				Json::parseRecord(log, fmap, &len);
				const QVariantList list = log.toList();
				QString serviceStr = QLatin1String("service");
				QString datetimeStr = QLatin1String("datetime");
				QString inStr = QLatin1String("in");
				QString htmlStr = QLatin1String("html");
				QString textStr = QLatin1String("text");
				QString senderNameStr = QLatin1String("senderName");
				QString newLine = QLatin1String("\n");
				for (int i = 0; i < list.size(); i++)
					{
						const QVariant &val = list.at(i);
						const QVariantMap message = val.toMap();
						history_service = message.value(serviceStr, false).toBool();
						history_date_time = QDateTime::fromString(message.value(datetimeStr).toString(), Qt::ISODate);
						history_in = message.value(inStr, false).toBool();
						historyMessage = message.value(htmlStr).toString();
						if (historyMessage.isEmpty()) {
							historyMessage = message.value(textStr).toString().toHtmlEscaped().
											 replace(QLatin1String("\n"), QLatin1String("<br>"));
						}
						QVariant sender = message.value(senderNameStr, history_in ? from_nickname : account_nickname);

						if ( history_date_time.date().day() == day )
						{
							history_in ? in++ : out++;
							if (history_service) {
								cursor.setCharFormat(serviceFont);
								cursor.insertText(serviceMessageTitle);
							} else {
								cursor.setCharFormat(history_in ? incomingFont : outgoingFont);
								cursor.insertText(sender.toString());
							}
							cursor.insertText(QLatin1Literal(" (")
											  % history_date_time.toString("dd.MM.yyyy hh:mm:ss")
											  % QLatin1Literal(")"));
							cursor.setCharFormat(defaultFont);
							cursor.insertText(newLine);
							if (search_word.isEmpty()) {
								cursor.insertHtml(historyMessage);
								cursor.insertText(newLine);
							} else {
////								QStringList parts = history_message.split(search_regexp);
////								for (int i = 0; i < )
//								int pos = 0, previous = 0;
//								while ((pos = searchRegexp.indexIn(historyMessage, pos)) != -1) {
//									cursor.setCharFormat(defaultFont);
//									cursor.insertHtml(historyMessage.mid(previous, pos));
//									cursor.setCharFormat(foundFont);
//									cursor.insertHtml(searchRegexp.cap(1));
//									pos += searchRegexp.matchedLength();
//									previous = pos;
//								}
//								cursor.setCharFormat(defaultFont);
								cursor.insertHtml(historyMessage.replace(searchRegexp, resultString));
								cursor.insertText(newLine);
							}
						}
					}
//				}
				cursor.endEditBlock();
				doc->setParent(ui.historyLog);
				ui.historyLog->setDocument(doc);
				file.close();
				if(search_word.isEmpty())
					ui.historyLog->moveCursor(QTextCursor::End);
				else
					ui.historyLog->find(search_word);
				ui.historyLog->verticalScrollBar()->setValue(ui.historyLog->verticalScrollBar()->maximum());
//				ui.historyLog->ensureCursorVisible();
			}
			ui.label_in->setText( tr( "In: %L1").arg( in ) );
			ui.label_out->setText( tr( "Out: %L1").arg( out ) );
			ui.label_all->setText( tr( "All: %L1").arg( in + out ) );
		}
	}
}

void HistoryWindow::on_searchButton_clicked()
{
	if (ui.accountComboBox->count() && ui.fromComboBox->count()) {
		if (m_search_word == ui.searchEdit->text()) {
			if (!ui.historyLog->find(m_search_word)) {
				ui.historyLog->moveCursor(QTextCursor::Start);
				ui.historyLog->find(m_search_word);
				ui.historyLog->ensureCursorVisible();
			}
		} else {
			fillDateTreeWidget(ui.fromComboBox->currentIndex(), ui.searchEdit->text().toLower());
		}
	}
}

void HistoryWindow::findPrevious()
{
	if (!ui.historyLog->find(m_search_word, QTextDocument::FindBackward)) {
		ui.historyLog->moveCursor(QTextCursor::End);
		ui.historyLog->find(m_search_word);
		ui.historyLog->ensureCursorVisible();
	}
}

}


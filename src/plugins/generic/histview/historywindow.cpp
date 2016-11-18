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
#include <qutim/icon.h>
#include <qutim/protocol.h>
#include <qutim/accountmanager.h>
#include <qutim/chatunit.h>
#include <QScrollBar>
#include <QStringBuilder>
#include <QDebug>
#include <qutim/systemintegration.h>
#include <qutim/shortcut.h>

namespace Core
{

static inline History *history()
{
	return History::instance();
}

static inline Account *findAccount(const History::AccountInfo &info)
{
	foreach (Account *account, AccountManager::instance()->accounts()) {
		if (account->protocol()->id() == info.protocol && account->id() == info.account)
			return account;
	}
	return nullptr;
}

static inline ChatUnit *findContact(const History::ContactInfo &info)
{
	if (Account *account = findAccount(info))
		return account->getUnit(info.contact);
	return nullptr;
}

HistoryWindow::HistoryWindow(const ChatUnit *unit)
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
	setIcons();

	m_unitInfo = unit ? History::info(unit) : History::ContactInfo();

	connect(ui.dateTreeWidget, &QTreeWidget::itemExpanded, this, &HistoryWindow::fillMonth);

	fillAccountComboBox();

	setParent(QApplication::activeWindow());
	setWindowFlags(windowFlags() | Qt::Window);

	QAction *action = new QAction(tr("Close"),this);
	connect(action, SIGNAL(triggered()), SLOT(close()));
	addAction(action);
	SystemIntegration::show(this);
}

void HistoryWindow::setUnit(const ChatUnit *unit)
{
	m_unitInfo = unit ? History::info(unit) : History::ContactInfo();
	History::AccountInfo accountInfo = m_unitInfo;
	int accountIndex = ui.accountComboBox->findData(QVariant::fromValue(accountInfo));
	if (accountIndex > -1) {
		ui.accountComboBox->setCurrentIndex(accountIndex);
		int contactIndex = ui.fromComboBox->findData(m_unitInfo.contact);
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
	history()->accounts().connect(this, [this] (const QVector<History::AccountInfo> &accounts) {
		for (const History::AccountInfo &info : accounts) {
			QString name = info.account;
			Icon icon(QLatin1String("im-") + info.protocol);

			if (Account *account = findAccount(info)) {
				if (!account->name().isEmpty())
				name += QStringLiteral(" - ") + account->name();
			}
			ui.accountComboBox->addItem(icon, name, QVariant::fromValue(info));
		}

		connect(ui.accountComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
				this, &HistoryWindow::fillContactComboBox);
		int accountIndex = ui.accountComboBox->findData(QVariant::fromValue<History::AccountInfo>(m_unitInfo));
		if (accountIndex < 0)
			fillContactComboBox(0);
		else
			ui.accountComboBox->setCurrentIndex(accountIndex);

		qDebug() << "accountIndex" << accountIndex;
	});
}

void HistoryWindow::fillContactComboBox(int index)
{
	if (ui.accountComboBox->count() == 0)
		return;
	auto accountInfo = ui.accountComboBox->itemData(index).value<History::AccountInfo>();
	ui.fromComboBox->clear();

	history()->contacts(accountInfo).connect(this, [this, accountInfo] (const QVector<History::ContactInfo> &contacts) {
		int index = ui.accountComboBox->currentIndex();
		auto currentAccountInfo = ui.accountComboBox->itemData(index).value<History::AccountInfo>();
		if (!(accountInfo == currentAccountInfo))
			return;

		disconnect(m_contactConnection);

		Account *account = findAccount(accountInfo);

		for (const History::ContactInfo &info : contacts) {
			QString name = info.contact;
			if (ChatUnit *unit = account ? account->getUnit(info.contact) : nullptr) {
				if (unit->title() != name)
					name += QStringLiteral(" - ") + unit->title();
			}

			ui.fromComboBox->addItem(name, QVariant::fromValue(info));
		}

		ui.fromComboBox->model()->sort(0);

		m_contactConnection = connect(ui.fromComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
									  this, &HistoryWindow::fillDateTreeWidget);
		int contactIndex = ui.fromComboBox->findData(QVariant::fromValue(m_unitInfo));
		if (!contactIndex)
			fillDateTreeWidget(0);
		else
			ui.fromComboBox->setCurrentIndex(contactIndex);
	});
}

void HistoryWindow::fillDateTreeWidget(int index)
{
	if (ui.fromComboBox->count() == 0)
		return;
	auto contactInfo = ui.fromComboBox->itemData(index).value<History::ContactInfo>();
	ui.dateTreeWidget->clear();

	setWindowTitle(QStringLiteral("%1 (%2)").arg(ui.fromComboBox->currentText(), ui.accountComboBox->currentText()));

	history()->months(contactInfo, m_search).connect(this, [this, contactInfo] (const QList<QDate> &months) {
		int index = ui.fromComboBox->currentIndex();
		auto currentContactInfo = ui.fromComboBox->itemData(index).value<History::ContactInfo>();
		if (!(currentContactInfo == contactInfo))
			return;

		QTreeWidgetItem *year = nullptr;
		QTreeWidgetItem *month = nullptr;
		Icon yearIcon(QStringLiteral("view-calendar-year"));

		for (const QDate &date : months) {
			if (!year || year->data(0, Qt::UserRole).toInt() != date.year()) {
				year = new QTreeWidgetItem(ui.dateTreeWidget);
				year->setText(0, QString::number(date.year()));
				year->setIcon(0, yearIcon);
				year->setData(0, Qt::UserRole, date.year());
				year->setExpanded(true);
			}

			month = new QTreeWidgetItem(year);
			month->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
			month->setText(0, QDate::longMonthName(date.month()));
			month->setIcon(0, Icon("view-calendar-month"));
			month->setData(0, Qt::UserRole, date);
			month->setExpanded(false);
		}

		if (month)
			ui.dateTreeWidget->setCurrentItem(month);
	});
}

void HistoryWindow::fillMonth(QTreeWidgetItem *monthItem)
{
	if (ui.fromComboBox->count() == 0)
		return;
	if (monthItem->data(0, Qt::UserRole).type() != QVariant::Date)
		return;

	auto contactIndex = ui.fromComboBox->currentIndex();
	auto contactInfo = ui.fromComboBox->itemData(contactIndex).value<History::ContactInfo>();
	auto month = monthItem->data(0, Qt::UserRole).toDate();

	history()->dates(contactInfo, month, m_search).connect(this, [this, contactInfo, month] (const QList<QDate> &dates) {
		int contactIndex = ui.fromComboBox->currentIndex();
		auto currentContactInfo = ui.fromComboBox->itemData(contactIndex).value<History::ContactInfo>();
		if (!(currentContactInfo == contactInfo))
			return;

		auto findChild = [] (QTreeWidgetItem *parent, const QVariant &value) -> QTreeWidgetItem * {
			if (!parent)
				return nullptr;

			for (int i = 0; i < parent->childCount(); ++i) {
				QTreeWidgetItem *child = parent->child(i);
				if (child->data(0, Qt::UserRole) == value)
					return child;
			}

			return nullptr;
		};

		auto monthItem = findChild(findChild(ui.dateTreeWidget->invisibleRootItem(), month.year()), month);
		if (!monthItem)
			return;

		for (const QDate &date : dates) {
			QTreeWidgetItem *item = new QTreeWidgetItem(monthItem);
			item->setText(0, QString::number(date.day()));
			item->setIcon(0, Icon("day"));
			item->setData(0, Qt::UserRole, date);
		}
	});
}

void HistoryWindow::on_dateTreeWidget_currentItemChanged(QTreeWidgetItem *dayItem, QTreeWidgetItem *)
{
	QTreeWidgetItem *monthItem = dayItem ? dayItem->parent() : nullptr;
	if (!dayItem || !monthItem)
		return;

	if (dayItem->data(0, Qt::UserRole).type() != QVariant::Date)
		return;
	if (monthItem->data(0, Qt::UserRole).type() != QVariant::Date)
		return;

	auto contactIndex = ui.fromComboBox->currentIndex();
	auto contactInfo = ui.fromComboBox->itemData(contactIndex).value<History::ContactInfo>();
	auto date = dayItem->data(0, Qt::UserRole).toDate();
	QDateTime from(date, QTime(0, 0));
	QDateTime to(date, QTime(23, 59, 59, 999));
	int count = std::numeric_limits<int>::max();

	history()->read(contactInfo, from, to, count).connect(this, [this, contactInfo, date] (const MessageList &messages) {
		int contactIndex = ui.fromComboBox->currentIndex();
		auto currentContactInfo = ui.fromComboBox->itemData(contactIndex).value<History::ContactInfo>();
		if (!(currentContactInfo == contactInfo))
			return;

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
		const QString serviceMessageTitle = tr("Service message");
		const QString resultString = QStringLiteral("<span style='background: #ffff00'>\\1</span>");
		cursor.beginEditBlock();

		Account *account = findAccount(contactInfo);
		ChatUnit *unit = findContact(contactInfo);

		QString accountNickname = account ? account->name() : contactInfo.account;
		QString fromNickname = unit ? unit->title() : contactInfo.contact;
		int in_count = 0;
		int out_count = 0;

		for (const Message &message : messages) {
			bool service = message.property("service", false);
			QDateTime time = message.time();
			bool incoming = message.isIncoming();
			QString historyMessage = message.html();
			QString sender = message.property("senderName", incoming ? fromNickname : accountNickname);

			incoming ? in_count++ : out_count++;
			if (service) {
				cursor.setCharFormat(serviceFont);
				cursor.insertText(serviceMessageTitle);
			} else {
				cursor.setCharFormat(incoming ? incomingFont : outgoingFont);
				cursor.insertText(sender);
			}
			cursor.insertText(QStringLiteral(" (")
							  % time.toString(QStringLiteral("dd.MM.yyyy hh:mm:ss"))
							  % QStringLiteral(")"));
			cursor.setCharFormat(defaultFont);
			cursor.insertText(QStringLiteral("\n"));
			if (m_search_word.isEmpty()) {
				cursor.insertHtml(historyMessage);
				cursor.insertText(QStringLiteral("\n"));
			} else {
				cursor.insertHtml(historyMessage.replace(m_search, resultString));
				cursor.insertText(QStringLiteral("\n"));
			}
		}
		cursor.endEditBlock();
		doc->setParent(ui.historyLog);
		ui.historyLog->setDocument(doc);
		if (m_search_word.isEmpty())
			ui.historyLog->moveCursor(QTextCursor::End);
		else
			ui.historyLog->find(m_search_word);
		ui.historyLog->verticalScrollBar()->setValue(ui.historyLog->verticalScrollBar()->maximum());

		ui.label_in->setText(tr("In: %L1").arg(in_count));
		ui.label_out->setText(tr("Out: %L1").arg(out_count));
		ui.label_all->setText(tr("All: %L1").arg(in_count + out_count));
	});
}

void HistoryWindow::on_searchButton_clicked()
{
	if (ui.accountComboBox->count() && ui.fromComboBox->count()) {
		QString searchWord = ui.searchEdit->text().toLower();
		if (m_search_word == searchWord) {
			if (!ui.historyLog->find(m_search_word)) {
				ui.historyLog->moveCursor(QTextCursor::Start);
				ui.historyLog->find(m_search_word);
				ui.historyLog->ensureCursorVisible();
			}
		} else {
			m_search_word = searchWord;
			m_search.setPattern(QLatin1Char('(') + QRegularExpression::escape(searchWord) + QLatin1Char(')'));
			m_search.setPatternOptions(QRegularExpression::MultilineOption | QRegularExpression::CaseInsensitiveOption);
			fillDateTreeWidget(ui.fromComboBox->currentIndex());
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

